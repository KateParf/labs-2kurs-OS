#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <iostream>
#include <Windows.h>
#include <system_error>
#include <string> 
#include <random>

using namespace std;

HANDLE consLock; // объект блокировки консоли
HANDLE hStdout; // хэндл стдаута (консоли)
string villagerName = "x";

void printError(string message) {
    WaitForSingleObject(consLock, INFINITE);

    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    //SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << "[client] [" << villagerName << "] ERROR: " << message << " : " << error << " : " << lastErrMsg << endl;
    //SetConsoleTextAttribute(hStdout, 7);

    ReleaseMutex(consLock);
}

void printLog(string message) {
    WaitForSingleObject(consLock, INFINITE);

    cout << "[client] [" << villagerName << "] " << message << endl;

    ReleaseMutex(consLock);
}

//-----------------

// масштабируем реальное время к виртуальному - 1 сутки = 2,4 сек
int TimeRealMinutesToModel(int mins) {
    return (1000 * mins / 60) / 10;
}

// Функция выбора времени до похода к дубу - минут с начала суток
int timeToGo() {
    int end = 24*60;
    int time = rand() % (end + 1);
    return time;
}

//-----------------

SOCKET sServer;

bool connect() {
    //Загрузка библиотеки
    WSAData wsaData; //создаем структуру для загрузки
    WORD DLLVersion = MAKEWORD(2, 1); // Версия библиотеки winsock 
    if (WSAStartup(DLLVersion, &wsaData) != 0) { // проверка подключения библиотеки
        printError("ошибка подключения к библиотеке сокетов");
        return false;
    }

    const char* server_addr = "127.0.0.1";
    const u_short server_port = 9999;

    //Заполняем информацию об адресе сокета
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(server_addr); // адрес
    addr.sin_port = htons(server_port); // порт
    addr.sin_family = AF_INET; //семейство протоколов

    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //проверка на подключение к серверу
    if (connect(sServer, (SOCKADDR*)&addr, sizeof(addr))) {
        printError("  ошибка подключения к серверу");
        return false;
    }
    printLog("  подключился к серверу");
    return true;
}

void disconnect() {
    closesocket(sServer);
    WSACleanup(); 
    printLog("  отключились от сервера");
}

//отдаем бочку серверу и в ответ получаем новое кол-во бочек
int giveBarell() {

    printLog("  отправляем на сервер новую бочку ...");
    char buffer[1024] = "";
    snprintf(buffer, sizeof(buffer), "NewBarrel [%s]   ", villagerName.c_str());
    if (send(sServer, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
        printError("  ошибка отправки сообщения на сервер");
        return -1;
    }
    printLog("  [v] отправил");

    int cnt = 0;
    printLog("  получаем с сервера новое кол-во бочек ...");
    if (recv(sServer, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
        printError("  ошибка получения данных с сервера");
        return -1;
    }
    string s = buffer; 
    cnt = stoi(s);

    printLog("  Получено: " + s);

    return cnt;
}


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    consLock = CreateMutex(NULL, FALSE, L"||consLock||");
    if (consLock == NULL) {
        cerr << "Ошибка создания мьютекса." << endl;
        return 1;
    }
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    //---
    if (argc > 1)
        villagerName = argv[1];

    // инициализируем рандом генератор
    random_device system_rand;
    int seed = system_rand();
    srand(seed);

    printLog(">>>> Родился житель");

    if (!connect()) {
        disconnect();
        return -1;
    }

    // цикл рабочей недели
    for (int day = 1; day < 6; day++) {
        printLog("-- Новый день: " + to_string(day));

        // В рандомное время дня идем пополнять бочки
        int timeBeforeGo = timeToGo();
        int timeAfterGo = (24*60) - timeBeforeGo;
        printLog("Ждет похода ... " + to_string(timeBeforeGo) + " мин.");
        Sleep(TimeRealMinutesToModel(timeBeforeGo));

        printLog("Понес бочку");
        int cntBarrels = giveBarell(); // сколько уже бочек под дубом
        printLog("Бочек : " + to_string(cntBarrels));

        if (cntBarrels > 5) {
            printLog("Звонит в колокол");

            HANDLE hEventBell = OpenEventA(
                EVENT_ALL_ACCESS,
                FALSE,  // не будет дочерних процессов
                "-Robin-Bell-" // уникальное имя объекта события
            );

            if (hEventBell == NULL) {
                printError("Ошибка открытия события. Наверное сервер не запущен.");
                return 1;
            } 

            SetEvent(hEventBell);
            CloseHandle(hEventBell);
        }
         
        printLog("Вернулся домой. Спит до конца суток : " + to_string(timeAfterGo) + " мин.");
        Sleep(TimeRealMinutesToModel(timeAfterGo));
    }

    printLog("<<<< Житель всё");
    disconnect();
    return 0;
}

