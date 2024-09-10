#include <iostream>
#include <Windows.h>
#include <system_error>
#include <string>
#include <tchar.h>


using namespace std;

void printError(string message) {
    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    cerr << message << " : " << error << " : " << lastErrMsg << endl;
}

void printLog(string message) {
    cout << message << endl;
}

// запуск ехе файла в новом процессе
PROCESS_INFORMATION RunProcess(string exeName, string params) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // Initialize the infomation.
    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    
    char commandline[_MAX_PATH * 2];
    snprintf(commandline, sizeof(commandline), "%s %s", exeName.c_str(), params.c_str());

    //printLog(commandline);
    if (! CreateProcessA(NULL, commandline, NULL, NULL, FALSE, 
        0, // не создавать новую новую консоль  писать в текущую 
        NULL, NULL, &si, &pi)) {
        cerr << "[runner] Ошибка запуска процесса: " << exeName << endl;
    }
    return pi;
}

int main() {
    setlocale(LC_ALL, "Russian");

    printLog("[R U N N E R]  С Т А Р Т   М Е Н Е Д Ж Е Р А");

    // запуск сервера
    auto spi = RunProcess("server.exe", "");
    Sleep(5000);

    // запуск клиентов
    const int clients = 12;

    PROCESS_INFORMATION clpis[clients];
    HANDLE clhnd[clients];
    for (int i = 0; i < clients; i++) {
        clpis[i] = RunProcess("client.exe", to_string(i+1));
        clhnd[i] = clpis[i].hProcess;
    }

    // ждем стопа клиентов
    WaitForMultipleObjects(clients, clhnd, true, INFINITE);
    Sleep(2000);

    for (int i = 0; i < clients; i++) {
        CloseHandle(clpis[i].hProcess);
        CloseHandle(clpis[i].hThread);
    }

    // останавливаем сервер
    Sleep(5000); 
    TerminateProcess(spi.hProcess, 0);
    CloseHandle(spi.hProcess);
    CloseHandle(spi.hThread);

    printLog("[R U N N E R]  Ф И Н И Ш   М Е Н Е Д Ж Е Р А");
}

