#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <iostream>
#include <Windows.h>
#include <system_error>
#include <string>
#include <chrono>

using namespace std;

HANDLE consLock; // объект блокировки консоли
HANDLE hStdout; // хэндл стдаута (консоли)

void printError(string message) {
    WaitForSingleObject(consLock, INFINITE);

    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    //SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << "[server] ERROR: " << message << " : " << error << " : " << lastErrMsg << endl;
    //SetConsoleTextAttribute(hStdout, 7);

    ReleaseMutex(consLock);
}

void printLog(string message, bool system = 1) {
    WaitForSingleObject(consLock, INFINITE);

    SetConsoleTextAttribute(hStdout, (system ? FOREGROUND_GREEN : FOREGROUND_BLUE) | FOREGROUND_INTENSITY);
    cout << "[server] " << message << endl;
    SetConsoleTextAttribute(hStdout, 7);

    ReleaseMutex(consLock);
}

//---------------

// масштабируем реальное время к виртуальному - 1 сутки = 2,4 сек
int TimeRealMinutesToModel(int mins) {
    return (1000 * mins / 60) / 10;
}

//--------------

const int days = 5;
const int roadTimeMin = 10; // Робин идет до дерева допустим 10 минут
int barrels = 0; // колво бочек (сейчас)
int barrelsTotal = 0; // колво бочек всего принесли
HANDLE barrelsLock; // объект блокировки бочек
HANDLE hEventBell;
int cntWalkers = 0; // кол-во ходящих разбойников
HANDLE walkersLock; // объект блокировки ходящих

HANDLE CreateFriendThread();

int getWalkers(int Inc) {
	int res = 0;
	WaitForSingleObject(walkersLock, INFINITE);
    cntWalkers += Inc;
	res = cntWalkers;
	ReleaseMutex(walkersLock);
	return res;
}

// поток жизненного цикла разбойника
DWORD WINAPI threadRobinBody(LPVOID lpvParam) {
	getWalkers(+1);

    bool isRobin = (bool)lpvParam;
    int localBarrels;
    int localBarrelsTotal = 0, localBarrelsTotalPrev = 0;
    int friendsCalls = 0; // статистика - сколько раз звали друзей
    std::chrono::system_clock::time_point lastTime = std::chrono::system_clock::now();

    printLog(string("[разбойник] Проснулся: ") + ((isRobin) ? "Робин" : "Друг"), 0);

    while (true) {
		// друг идет сразу, а Робин ждет колокол
        if (!isRobin || (WaitForSingleObject(hEventBell, TimeRealMinutesToModel(24*60)) == WAIT_OBJECT_0)) {
            printLog("[разбойник] " + string((isRobin) ? "Робин" : "Друг") + string(" пошел за бочкой"), 0);
            if (isRobin) 
                ResetEvent(hEventBell);

            do {
                // идем к дубу
                Sleep(TimeRealMinutesToModel(roadTimeMin));

				// забираем бочку
                // блок мьютекса - для потокобезопасного обращения к переменной
                WaitForSingleObject(barrelsLock, INFINITE);
				if (barrels > 0) {
					barrels--;
					localBarrels = barrels;
				} else {
					localBarrels = -1; // флаг что бочек нет
				}
                ReleaseMutex(barrelsLock);
				
				if (localBarrels >= 0)
					printLog("[разбойник] " + string((isRobin) ? "Робин" : "Друг") + string(" забрал бочку. Осталось: ") + to_string(localBarrels), 0);
				else
					printLog("[разбойник] " + string((isRobin) ? "Робин" : "Друг") + string(" Бочек нет, зря ходили :"), 0);

                // идем от дуба
                Sleep(TimeRealMinutesToModel(roadTimeMin));

                // приходит обратно, если бочек все еще > 5 - зовет 2х друзей (если еще не звал)
                if ((localBarrels > 5) && isRobin && (getWalkers(0) == 1)) {
                    printLog("[разбойник] Робин зовет друзей", 0);
                    friendsCalls++;
                    HANDLE f1 = CreateFriendThread();
                    HANDLE f2 = CreateFriendThread();
                    CloseHandle(f1);
                    CloseHandle(f2);
                }

            } while (localBarrels >= getWalkers(0));

            lastTime = std::chrono::system_clock::now();
        }

        if (!isRobin) {
			printLog("[разбойник] Друг всё, пока", 0);
			break;
        }

		// если прошло 2 суток и никто не приходил то пишем статистику
        auto endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = endTime - lastTime;
        if (TimeRealMinutesToModel(2*24*60) < elapsed_seconds.count() * 1000) {
            DWORD dwWaitResult = WaitForSingleObject(barrelsLock, INFINITE);
            localBarrelsTotal = barrelsTotal;
            ReleaseMutex(barrelsLock);

            if (localBarrelsTotal != localBarrelsTotalPrev) {
                printLog("[СТАТИСТИКА] ====", 0);
                printLog("[СТАТИСТИКА] Бочек принесли (шт): " + to_string(localBarrelsTotal), 0);
                printLog("[СТАТИСТИКА] Звал друзей (раз): " + to_string(friendsCalls), 0);
                printLog("[СТАТИСТИКА] ====", 0);
                localBarrelsTotalPrev = localBarrelsTotal;
            }
        }

    }

	getWalkers(-1);
    return 1;
}

// Функция создания потока Робина или друга Робина
HANDLE CreateRobinOrFriendThread(bool isRobin) {
    DWORD dwThreadId = 0;
    HANDLE hThread = CreateThread(
        NULL,              // no security attribute 
        0,                 // начальный размер стека
        threadRobinBody,   
        (LPVOID)isRobin,   // параметр потока - передаем в поток признак Робин не Робин
        0,                 // flags: 0 - запуск сразу
        &dwThreadId);      
    if (hThread == NULL) {
        printError("[разбойник] Не удалось создать поток Робина");
        // написать обработку
    }
    return hThread;
}

// Функция создания потока Робина
HANDLE CreateRobinThread() {
    return CreateRobinOrFriendThread(true);
}

// Функция создания потока друга Робина
HANDLE CreateFriendThread() {
    return CreateRobinOrFriendThread(false);
}

//--------------

DWORD WINAPI clientTalk(LPVOID lpvParam) {
    SOCKET connection = (SOCKET)lpvParam;
    char buffer[1024] = "";
    int localBarrels;
    int bytes;

    int connId = (int)connection;
    string talkPrefix = to_string(connId) + " [talk]";
    
    printLog(talkPrefix + "  <><><>   запущен   поток   общения   с   клиентом  <><><>");

    while (true) {

        // получить от клиента сообщение
        printLog(talkPrefix + " << читаем сообщение с клиента ...");
        if ((bytes = recv(connection, buffer, sizeof(buffer), 0)) == SOCKET_ERROR) {
            printError(talkPrefix + " << ошибка получения данных с сервера");
            break;
        }

        if (bytes == 0) break; // выходим когда клиент прервал конект

        string s = buffer;
        printLog(talkPrefix + " << с клиента пришло: " + s);
        /* проверить
        if (s.rfind("NewBarrel ", 0) != 0) {
            printError(talkPrefix + " << клиент прислал непонятное сообщение: [ " + s + " ](" + to_string(bytes) + ") ");
            break;
        }*/

        talkPrefix = to_string(connId) + " [talk] " + s.substr(10, 6);

        // увеличиваем количество бочек
        // блок мьютекса
        DWORD dwWaitResult = WaitForSingleObject(barrelsLock, INFINITE);
        barrels++;
        barrelsTotal++;
        localBarrels = barrels;
        ReleaseMutex(barrelsLock);

        // отослать колво бочек
        printLog(talkPrefix + " >> клиент кол-во бочек: " + to_string(localBarrels));
        strcpy_s(buffer, (to_string(localBarrels)).c_str());
        if (send(connection, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            printError(talkPrefix + " << ошибка отправки на клиент ");
            break;
        }
        printLog(talkPrefix + " [v] отправка");

    }

    closesocket(connection);
    printLog(talkPrefix + " >< конец связи");

    return 1;
}


DWORD WINAPI ListenerProc(LPVOID lpvParam) {
    printLog("[сервер] старт слушателя");

    //Загрузка библиотеки
    WSAData wsaData; //создаем структуру для загрузки
    WORD DLLVersion = MAKEWORD(2, 1); // Версия библиотеки winsock
    if (WSAStartup(DLLVersion, &wsaData) != 0) { // проверка подключения библиотеки
        printError("[сервер] ошибка подключения к библиотеке сокетов");
        return false;
    }

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //сокет для прослушивания порта

    //Заполняем информацию об адресе сокета
    const char* server_addr = "127.0.0.1";
    const u_short server_port = 9999;

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(server_addr);
    addr.sin_port = htons(server_port);
    addr.sin_family = AF_INET;

    if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        // привязка адреса сокету
        printError("ошибка функции bind");
        closesocket(sListen);
        // прекращает использование библиотеки DLL Winsock 2
        WSACleanup();
        return 1;
    } 
    printLog("[сервер] сокет связан с локальным адресом");

    if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) {
        printError("ошибка функции listen");
        closesocket(sListen);
        WSACleanup();
        return 1;
    }
    printLog("[сервер] слушаем клиента");

    
    while (true) {
        SOCKET newConnection = accept(sListen, NULL, NULL); //сокет для соединения с клиентом

        if (!newConnection) {
            // проверяем, произошло ли соединение с клиентом
            printError("[сервер] ошибка соединения с клиентом");
            closesocket(sListen);
            WSACleanup();
            return 1;
        }
        else {
            printLog("[сервер] соединились с клиентом");
            
            // запускаем новый поток для общения с клиентом
            DWORD tid; 
            HANDLE thread = CreateThread(NULL, 0, clientTalk, (void*)newConnection, 0, &tid); //Создание потока для общения с клиентом
            if (thread == NULL) { // Ошибка создания потока
                printError("[сервер] ошибка создания потока связи с клиентом");
            }

        }
    }
}

HANDLE CreateListener() {
    DWORD dwThreadId = 0;
    HANDLE hThread = CreateThread(
        NULL,              // no security attribute 
        0,                 // начальный размер стека
        ListenerProc,
        NULL,
        0,                 // flags: 0 - запуск сразу
        &dwThreadId);
    if (hThread == NULL) {
        printError("(поток) Не удалось создать поток слушателя");
        // написать обработку
    }
    return hThread;
}

//------------

int main() {
    setlocale(LC_ALL, "Russian");

    consLock = CreateMutex(NULL, FALSE, L"||consLock||");
    if (consLock == NULL) {
        cerr << "Ошибка создания мьютекса консоли." << endl;
        return 1;
    }
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    printLog("Старт сервера");

    barrelsLock = CreateMutex(NULL, FALSE, NULL);
    if (barrelsLock == NULL) {
        printError("Ошибка создания мьютекса бочек.");
        return 1;
    }

    walkersLock = CreateMutex(NULL, FALSE, NULL);
    if (walkersLock == NULL) {
        printError("Ошибка создания мьютекса разбойников.");
        return 1;
    }

    hEventBell = CreateEventA(
        NULL,  // дескриптор не может наследоваться дочерними процессами.
        TRUE,  // создание события со сбросом вручную(True) или с автосбросом(False)
        FALSE,  // начальное состояние события
        "-Robin-Bell-" // уникальное имя объекта события
    );
    if (hEventBell == NULL) {
        printError("Ошибка создания события. Возможно другой сервер запущен.");
        return 1;
    }

	//------
    // старт потока Робин
    HANDLE thrRobin = CreateRobinThread();

    // запуск сервера слушателя
    HANDLE listener = CreateListener();

    // ждем пока Робин отработает Х дней и завершаемся -- теоретически в идеале))
    WaitForSingleObject(thrRobin, INFINITE);

    CloseHandle(thrRobin);
    CloseHandle(listener);
    WSACleanup();

    CloseHandle(hEventBell);
    CloseHandle(walkersLock);
    CloseHandle(barrelsLock);

    //----

    printLog("Финиш сервера");
}
