#include <iostream>
#include <Windows.h>
#include <thread>
using namespace std;

/*
Требуется запустить несколько процессов-клиентов. !реализация через именованные каналы!
При запуске сервера, он должен закрыть все открытые таким образом
процессы-клиенты и не давать запускаться новым клиентам.
Также он не должен давать запуститься запустить более чем одной копии себя самого.
Клиенты до запуска сервера должны считать каждую
секунду числа от одного до 1.000.000. От 3 до 5 клиентов достаточно.
*/

// печатает сообщение об ошибке, с расшифровкой последней ошибки
void printError(string message) {
    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    cerr << message << " : " << error << " : " << lastErrMsg << endl;
}


DWORD WINAPI InstanceThread(LPVOID); // предварительно объявляем ф-цию потока

int main() {
    setlocale(LC_ALL, "Russian");

    wchar_t pipeName[] = L"\\\\.\\pipe\\mynamedpipe";
    HANDLE hNamedPipe;
    bool fSuccess;          

    bool owner = false; // флаг что мы являемся создателем канала (проверка на 2й сервер)

    HANDLE hThread;
    DWORD  dwThreadId = 0;

    while (true) {
        // (1) создаем именованный канал
        cout << "Создаем канал" << endl;
        hNamedPipe = CreateNamedPipe(
            pipeName,                      
            PIPE_ACCESS_DUPLEX,          
            PIPE_TYPE_MESSAGE | PIPE_WAIT, // синхронная передача сообщений
            100,                             // макс колво экземпляров канала 
            100,                           // размер выходного буфера по умолчанию
            100,                           // размер входного буфера по умолчанию
            1*60*1000,                     // 1 мин
            NULL                           // защита по умолчанию
        );

        // проверяем на успешное создание канала
        // (когда клиент подключился и занял канал -мы создаем еще 1 для следующего клиента, 
        // в этот момент есть типа ошибка что канал существует, но если мы овнер то это норм
        DWORD error = GetLastError();
        if ((! owner) && ((error != 0) || (hNamedPipe == INVALID_HANDLE_VALUE))) {
            // если код ошибки не 0, (и мы не владелец канала), значит другой сервер запущен
            printError("Другой сервер уже запущен. Выходим!  error: ");
            return 1;
        } 
        
        cout << "Запустили сервер " << endl;
        owner = true;

        // (2) ждем пока клиент свяжется с каналом
        cout << "Ожидаем подключение клиентов ..." << endl;
        fSuccess = ConnectNamedPipe(
            hNamedPipe, // дескриптор канала
            NULL        // связь синхронная
        );

        if (!fSuccess) {
            printError("Не удалось дождаться клиентов. Выходим. Пока.");
            CloseHandle(hNamedPipe);
            return -1; // совсем выходим
        }


        // (3) подключаем клиента
        // создаем для его обработки выделенный поток и запускаем его
        cout << endl << "Есть клиент !!!" << endl;
        hThread = CreateThread(
            NULL,              // no security attribute 
            0,                 // наачальный размер стека
            InstanceThread,    // thread proc функция потока
            (LPVOID)hNamedPipe,    //параметр потока передаем в поток хендл канала
            0,                 // flags: 0 - запуск сразу
            &dwThreadId);      // thread ID 

        if (hThread == NULL) {
            printError("(поток) Не удалось создать поток");
            CloseHandle(hNamedPipe);
            continue;
        }

        // очищаем хендл потока (очищаем ресурсы)
        // это не стопит сам поток. он остановится сам как доработает до конца
        cout << "(поток) закрываем поток" << endl;
        CloseHandle(hThread);
        // а канал тут не закрываем - он закроется в потоке
    }

    return 0;
}

// поток обработки запросов клиента
DWORD WINAPI InstanceThread(LPVOID lpvParam) { 
    bool fSuccess;          
    HANDLE hNamedPipe;        // хэндл им. канала который передали в поток
    DWORD cbIO;               // Количество записанных байт
    char data[] = "stop\0";   // Данные, которые надо передать

    hNamedPipe = (HANDLE)lpvParam;

    cout << "Клиент подключен" << endl;

    // пишем в именованный канал
    fSuccess = WriteFile(
        hNamedPipe,       // дескриптор канала
        &data,            // данные
        strlen(data) + 1, // размер данных
        &cbIO,            // количество записанных байтов
        NULL              // синхронная запись
    );
    if (!fSuccess) {
        printError("Ошибка при записи в канал");
    }
    else {
        cout << "Данные успешно отосланы на клиент. " << cbIO << " байт" << endl;
        FlushFileBuffers(hNamedPipe);
    }

    // отключаем клиента от канала
    cout << "Отключаем клиента..." << endl;
    fSuccess = DisconnectNamedPipe(hNamedPipe);
    cout << "Отключили." << endl;

    cout << "Закрываем канал" << endl;
    CloseHandle(hNamedPipe);

    cout << "(поток) завершился" << endl;
    return 1;
}
