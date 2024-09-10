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

void printError(string message) {
    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    cerr << message << " : " << error << " : " << lastErrMsg << endl;
}


int main() {
    setlocale(LC_ALL, "Russian");

    HANDLE hNamedPipe;
    bool fSuccess;          // Переменная для проверки корректности операций
    wchar_t pipeName[] = L"\\\\.\\pipe\\mynamedpipe";

    // создаем именованный канал
    hNamedPipe = CreateNamedPipe(
        pipeName,                      // имя канала
        PIPE_ACCESS_DUPLEX,            // читаем и пишем
        PIPE_TYPE_MESSAGE | PIPE_WAIT, // синхронная передача сообщений
        1,                              // макс колво экземпляров канала ///- задаем 1 чтоб запскался только 1 сервер
        100,                            // размер выходного буфера по умолчанию
        100,                            // размер входного буфера по умолчанию
        1*60*1000,                      // ждем связи с клиентом 1 мин
        NULL                            // защита по умолчанию
    );

    // проверяем на успешное создание
    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        // если другой сервер запущен
        DWORD error = GetLastError();
        if (error == 231) { // код ошибкм канал занят
            cerr << "Другой сервер уже запущен. Выходим!" << endl;
        } else
            printError("Ошибка при создании канала");
        return 1;
    }

    while (true) {
        // ждем пока клиент свяжется с каналом
        cout << endl << "Ожидаем подключение клиентов ..." << endl;
        fSuccess = ConnectNamedPipe(
            hNamedPipe, // дескриптор канала
            NULL        // связь синхронная
        );

        if (!fSuccess) {
            printError("Не удалось дождаться клиентов.");
            break;
        }

        cout << "Есть клиент !!!" << endl;

        // отключаем клиента от канала
        fSuccess = DisconnectNamedPipe(hNamedPipe);
        cout << "Отключили клиента." << endl;
    }

    // закрываем дескриптор канала
    CloseHandle(hNamedPipe);
    return 0;
}
