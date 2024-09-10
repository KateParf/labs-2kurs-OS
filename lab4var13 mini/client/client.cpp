#include <iostream>
#include <Windows.h>
#include <thread>
#include <system_error>

using namespace std;

/*
“ребуетс€ запустить несколько процессов-клиентов. !реализаци€ через именованные каналы!
ѕри запуске сервера, он должен закрыть все открытые таким образом
процессы-клиенты и не давать запускатьс€ новым клиентам.
“акже он не должен давать запуститьс€ запустить более чем одной копии себ€ самого.
 лиенты до запуска сервера должны считать каждую
секунду числа от одного до 1.000.000. ќт 3 до 5 клиентов достаточно.
*/

void printError(string message) {
    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    cerr << message << " : " << error << " : " << lastErrMsg << endl;
}


int main() {
    setlocale(LC_ALL, "Russian");

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); // хэндл стдаута (консоли)
    HANDLE hNamedPipe;          // канал
    bool fSuccess;              // ѕеременна€ дл€ проверки корректности операций
    wchar_t pipeName[] = L"\\\\.\\pipe\\mynamedpipe";

    //--- основной цикл

    int cntSec = 0;
    const int waitSewconds = 1000000;
    // считаем до миллиона и выходим (если нет сервера)
    // если есть выходим сразу как почуем сервер
    while (cntSec <= waitSewconds) {

        // св€зываемс€ с именованным каналом
        cout << endl << "св€зываемс€ с именованным каналом" << endl;
        hNamedPipe = CreateFile(
            pipeName,             // им€ канала
            GENERIC_READ,         // чтение или запись в канал
            0,                    // no sharing 
            NULL,                 // защита по умолчанию
            OPEN_EXISTING,        // открываем существующий канал
            0,                    // флаги и атрибуты по умолчанию
            NULL                  // дополнительных атрибутов нет
        );

        /*≈сли функци€ выполн€етс€ успешно, возвращаемое значение представл€ет собой открытый дескриптор дл€
        указанного файла, устройства, именованного канала или почтового слота.
        ≈сли функци€ завершаетс€ неудачно, возвращаетс€ значение INVALID_HANDLE_VALUE.
        ƒополнительные сведени€ об ошибке можно получить, вызвав GetLastError.*/

        // провер€ем св€зь с каналом
        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            // если мы тут то была ошибка с каналом
            printError("ќшибка при подключении к серверу");
        }
        else {
            cout << "ѕодключились к серверу" << endl;
            cout << "—ервер запущен. ¬се на выход!" << endl;
            exit(0);
        }

        // если ничего не получили то ждем секунду и на новый цикл
        Sleep(1000); // 1000=1 sec
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY); // just for fun
        cout << cntSec << "сек. прошло" << endl;
        SetConsoleTextAttribute(hStdout, 7);
        cntSec++;

        // закрываем канал
        cout << "закрываем канал" << endl;
        CloseHandle(hNamedPipe);
    }

    cout << "—ервер не запущен, мы ждали долго! ƒо свидани€" << endl;
    return 1;
}