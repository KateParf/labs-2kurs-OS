#include <iostream>
#include <Windows.h>
#include <system_error>
#include <string>
#include <tchar.h>


using namespace std;

//---

HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;

//---

void printError(string message) {
    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    cerr << message << " : " << error << " : " << lastErrMsg << endl;
}

void printLog(string message) {
    cout << message << endl;
}


// запуск ехе файла в новом процессе
PROCESS_INFORMATION RunProcess(string exeName, string params, HANDLE StdOutReadHnd) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // Initialize the infomation.
    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.hStdError  = g_hChildStd_OUT_Wr;
    si.dwFlags |= STARTF_USESTDHANDLES;

    char commandline[_MAX_PATH * 2];
    snprintf(commandline, sizeof(commandline), "%s %s", exeName.c_str(), params.c_str());

    //printLog(commandline);
    if (! CreateProcessA(NULL, commandline, NULL, NULL, TRUE, 
        0, // не создавать новую новую консоль  писать в текущую 
        NULL, NULL, &si, &pi)) {
        printError("[runner] Ошибка запуска процесса: " + exeName);
    }
    return pi;
}
//--------------

// поток для чтения стрима с выводом от дочерних процессов
DWORD WINAPI ReadConsoleProc(LPVOID lpvParam) {
    const int bufsize = 9999;
    char buffer[bufsize];
    DWORD reads;
    BOOL bSuccess = FALSE;

    printLog("запущен поток чтения вывода дочерних процессов");

    while (true) {
        memset(&buffer, 0, bufsize);
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, buffer, bufsize, &reads, NULL);
        if (bSuccess && (reads > 0)) {
            string s = buffer;
            printLog(s);
        } else
            Sleep(100);
    }

    return 1;
}

// создаем каналы для чтения вывода дочерних процессов
// и запускаем поток чтения канала
HANDLE CreateReadConsole() {

    // https://stackoverflow.com/questions/35969730/how-to-read-output-from-cmd-exe-using-createprocess-and-createpipe
    // https://learn.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
        printError("[runner] Ошибка создания pipe для процесса");

    // запускаем фоновый поток чтения канала
    DWORD dwThreadId = 0;
    HANDLE hThread = CreateThread(
        NULL,              // no security attribute 
        0,                 // начальный размер стека
        ReadConsoleProc,
        NULL,
        0,                 // flags: 0 - запуск сразу
        &dwThreadId);
    if (hThread == NULL) {
        printError("[runner] (поток) Не удалось создать поток читателя консоли");
        // написать обработку
    }
    return hThread;
}

int main() {
    setlocale(LC_ALL, "Russian");
    printLog("[R U N N E R]  С Т А Р Т   М Е Н Е Д Ж Е Р А");

    CreateReadConsole();

    // запуск сервера
    auto spi = RunProcess("server.exe", "", g_hChildStd_OUT_Wr);
    Sleep(5000);

    // запуск клиентов
    const int clients = 12;

    PROCESS_INFORMATION clpis[clients];
    HANDLE clhnd[clients];
    for (int i = 0; i < clients; i++) {
        clpis[i] = RunProcess("client.exe", to_string(i+1), g_hChildStd_OUT_Wr);
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

    // закрываем потоки
    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_OUT_Rd);

    printLog("[R U N N E R]  Ф И Н И Ш   М Е Н Е Д Ж Е Р А");
}

