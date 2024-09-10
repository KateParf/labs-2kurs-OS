#include <iostream>
#include <Windows.h>
#include <thread>
#include <system_error>

using namespace std;

/*
Требуется запустить несколько процессов-клиентов. !реализация через именованные каналы!
При запуске сервера, он должен закрыть все открытые таким образом
процессы-клиенты и не давать запускаться новым клиентам.
Также он не должен давать запуститься запустить более чем одной копии себя самого.
Клиенты до запуска сервера должны считать каждую
секунду числа от одного до 1.000.000. От 3 до 5 клиентов достаточно.
*/

HANDLE hStdout; // хэндл стдаута (консоли)

// печатает (красивое) сообщение об ошибке, с расшифровкой последней ошибки
void printError(string message) {
    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);

    SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY); // just for fun
    cerr << message << " : " << error << " : " << lastErrMsg << endl;
    SetConsoleTextAttribute(hStdout, 7);
}


int main() {
    setlocale(LC_ALL, "Russian");

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    wchar_t pipeName[] = L"\\\\.\\pipe\\mynamedpipe";
    HANDLE hNamedPipe;          // канал
    bool fSuccess;              // Переменная для проверки корректности операций
    DWORD cbIO;                 // Количество считанных/записанных байт
    const int BUFSIZE = 512;
    char data[BUFSIZE];

    //--- основной цикл

    int cntSec = 0;
    const int waitSewconds = 1000*1000;
    //  считаем до миллиона и выходим (если нет сервера)
    // если есть выходим сразу
    while (cntSec <= waitSewconds) {

        if (cntSec > 0) {
            // ждем секунду и на новый цикл
            Sleep(1000); // 1000=1 sec
            SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // just for fun
            cout << cntSec << "сек. прошло" << endl;
            SetConsoleTextAttribute(hStdout, 7);
        }
        cntSec++;

        // связываемся с именованным каналом
        cout << endl << "Связываемся с сервером" << endl;
        hNamedPipe = CreateFile(
            pipeName,             // имя канала
            GENERIC_READ,  
            0,                    // no sharing 
            NULL,                 // защита по умолчанию
            OPEN_EXISTING,        // открываем существующий канал
            0,                    // флаги и атрибуты по умолчанию
            NULL                  // дополнительных атрибутов нет
        );

        /*Если функция выполняется успешно, возвращаемое значение представляет собой открытый дескриптор для
        указанного файла, устройства, именованного канала или почтового слота.
        Если функция завершается неудачно, возвращается значение INVALID_HANDLE_VALUE.
        Дополнительные сведения об ошибке можно получить, вызвав GetLastError.*/

        // проверяем связь с каналом
        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            // если мы тут то была ошибка с каналом
            printError("  сервер не найден");
            CloseHandle(hNamedPipe);
            continue;
        } 

        cout << "Подключились к серверу" << endl;

        // Со стороны процесса-клиента перед подсоединением к каналу логично проверить, занят канал или свободен
        fSuccess = WaitNamedPipe(pipeName, 100);
        if (!fSuccess) {
            printError("[x] канал занят");
            CloseHandle(hNamedPipe);
            continue;
        }

        // читаем данные из канала
        cout << "Читаем с сервера ..." << endl;
        fSuccess = ReadFile(
            hNamedPipe,       // Канал
            &data,            // Куда записываем данные
            BUFSIZE * sizeof(char),  // size of buffer 
            &cbIO,            // Сколько прочитали байт
            NULL);            // Не асинхронное чтение

        // если что то получили то выходим
        if (fSuccess) {
            if (cbIO > 0) {
                SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // just for fun
                cout << "получено с сервера: " << data << endl;
                cout << "Сервер велел остановиться. Все на выход!" << endl;
                SetConsoleTextAttribute(hStdout, 7);
                exit(0);
            }
            else
                printError(".. ничего не получили");
        }
        else
            printError("Ошибка чтения с сервера");

        // закрываем канал
        cout << "закрываем канал" << endl;
        CloseHandle(hNamedPipe);
    }

    cout << "Сервер не запущен, мы ждали долго! До свидания" << endl;
    return 1;
}