#include <iostream>
#include <Windows.h>
#include <thread>
#include <system_error>

using namespace std;

/*
��������� ��������� ��������� ���������-��������. !���������� ����� ����������� ������!
��� ������� �������, �� ������ ������� ��� �������� ����� �������
��������-������� � �� ������ ����������� ����� ��������.
����� �� �� ������ ������ ����������� ��������� ����� ��� ����� ����� ���� ������.
������� �� ������� ������� ������ ������� ������
������� ����� �� ������ �� 1.000.000. �� 3 �� 5 �������� ����������.
*/

HANDLE hStdout; // ����� ������� (�������)

// �������� (��������) ��������� �� ������, � ������������ ��������� ������
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
    HANDLE hNamedPipe;          // �����
    bool fSuccess;              // ���������� ��� �������� ������������ ��������
    DWORD cbIO;                 // ���������� ���������/���������� ����
    const int BUFSIZE = 512;
    char data[BUFSIZE];

    //--- �������� ����

    int cntSec = 0;
    const int waitSewconds = 1000*1000;
    //  ������� �� �������� � ������� (���� ��� �������)
    // ���� ���� ������� �����
    while (cntSec <= waitSewconds) {

        if (cntSec > 0) {
            // ���� ������� � �� ����� ����
            Sleep(1000); // 1000=1 sec
            SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // just for fun
            cout << cntSec << "���. ������" << endl;
            SetConsoleTextAttribute(hStdout, 7);
        }
        cntSec++;

        // ����������� � ����������� �������
        cout << endl << "����������� � ��������" << endl;
        hNamedPipe = CreateFile(
            pipeName,             // ��� ������
            GENERIC_READ,  
            0,                    // no sharing 
            NULL,                 // ������ �� ���������
            OPEN_EXISTING,        // ��������� ������������ �����
            0,                    // ����� � �������� �� ���������
            NULL                  // �������������� ��������� ���
        );

        /*���� ������� ����������� �������, ������������ �������� ������������ ����� �������� ���������� ���
        ���������� �����, ����������, ������������ ������ ��� ��������� �����.
        ���� ������� ����������� ��������, ������������ �������� INVALID_HANDLE_VALUE.
        �������������� �������� �� ������ ����� ��������, ������ GetLastError.*/

        // ��������� ����� � �������
        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            // ���� �� ��� �� ���� ������ � �������
            printError("  ������ �� ������");
            CloseHandle(hNamedPipe);
            continue;
        } 

        cout << "������������ � �������" << endl;

        // �� ������� ��������-������� ����� �������������� � ������ ������� ���������, ����� ����� ��� ��������
        fSuccess = WaitNamedPipe(pipeName, 100);
        if (!fSuccess) {
            printError("[x] ����� �����");
            CloseHandle(hNamedPipe);
            continue;
        }

        // ������ ������ �� ������
        cout << "������ � ������� ..." << endl;
        fSuccess = ReadFile(
            hNamedPipe,       // �����
            &data,            // ���� ���������� ������
            BUFSIZE * sizeof(char),  // size of buffer 
            &cbIO,            // ������� ��������� ����
            NULL);            // �� ����������� ������

        // ���� ��� �� �������� �� �������
        if (fSuccess) {
            if (cbIO > 0) {
                SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // just for fun
                cout << "�������� � �������: " << data << endl;
                cout << "������ ����� ������������. ��� �� �����!" << endl;
                SetConsoleTextAttribute(hStdout, 7);
                exit(0);
            }
            else
                printError(".. ������ �� ��������");
        }
        else
            printError("������ ������ � �������");

        // ��������� �����
        cout << "��������� �����" << endl;
        CloseHandle(hNamedPipe);
    }

    cout << "������ �� �������, �� ����� �����! �� ��������" << endl;
    return 1;
}