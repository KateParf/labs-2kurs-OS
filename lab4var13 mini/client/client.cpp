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

void printError(string message) {
    DWORD error = GetLastError();
    string lastErrMsg = system_category().message(error);
    cerr << message << " : " << error << " : " << lastErrMsg << endl;
}


int main() {
    setlocale(LC_ALL, "Russian");

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); // ����� ������� (�������)
    HANDLE hNamedPipe;          // �����
    bool fSuccess;              // ���������� ��� �������� ������������ ��������
    wchar_t pipeName[] = L"\\\\.\\pipe\\mynamedpipe";

    //--- �������� ����

    int cntSec = 0;
    const int waitSewconds = 1000000;
    // ������� �� �������� � ������� (���� ��� �������)
    // ���� ���� ������� ����� ��� ������ ������
    while (cntSec <= waitSewconds) {

        // ����������� � ����������� �������
        cout << endl << "����������� � ����������� �������" << endl;
        hNamedPipe = CreateFile(
            pipeName,             // ��� ������
            GENERIC_READ,         // ������ ��� ������ � �����
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
            printError("������ ��� ����������� � �������");
        }
        else {
            cout << "������������ � �������" << endl;
            cout << "������ �������. ��� �� �����!" << endl;
            exit(0);
        }

        // ���� ������ �� �������� �� ���� ������� � �� ����� ����
        Sleep(1000); // 1000=1 sec
        SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY); // just for fun
        cout << cntSec << "���. ������" << endl;
        SetConsoleTextAttribute(hStdout, 7);
        cntSec++;

        // ��������� �����
        cout << "��������� �����" << endl;
        CloseHandle(hNamedPipe);
    }

    cout << "������ �� �������, �� ����� �����! �� ��������" << endl;
    return 1;
}