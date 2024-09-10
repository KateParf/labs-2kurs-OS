#include <windows.h>
#include <iostream>
#include <thread>
#define CONNECTING_STATE 0
#define READING_STATE 1
#define WRITING_STATE 2
#define INSTANCES 1
#define PIPE_TIMEOUT 5000
#define BUFSIZE 512
using namespace std;

typedef struct
{
	OVERLAPPED oOverlap; //���������, ����������� ����������� ����������� ��������
	HANDLE hPipeInst; //���������� ������
	DWORD cbRead; //������ ���-�� ����������� ����
	unsigned int size;
	unsigned int dwState; //��������� ������ ������: ������, ������ ��� �����������
	bool fPendingIO; //���������, ������������, ��������� �� � ������ ������
} PIPEINST;

void DisconnectAndReconnect(unsigned int);
bool ConnectToNewClient(HANDLE, LPOVERLAPPED);
PIPEINST Pipe[INSTANCES];
HANDLE hEvents[INSTANCES];

void print_time() {
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	printf("%02d:%02d:%02d\t", lt.wHour, lt.wMinute, lt.wMilliseconds);
}


int xxmain(void)
{
	unsigned int i, dwWait, dwErr;
	DWORD cbRet;
	bool fSuccess;
	bool pSuccess = true;
	//������������� ���� ���������� ������
	for (i = 0; i < INSTANCES; i++) {
		//������� ��� ����������
		hEvents[i] = CreateEvent(
			NULL, //������� ������
			true, //������ ����������
			true, //��������� ��������� - �������
			NULL); //���
		if (hEvents[i] == NULL) {
			print_time();
			cout << "[ERROR] CreateEvent failed with " << GetLastError() << '\n';
			return 0;
		}
		//����������� ��������� OVERLAP �������
		Pipe[i].oOverlap.hEvent = hEvents[i];
		//�������������� ��� �����
		Pipe[i].hPipeInst = CreateNamedPipe(
			L"\\\\.\\pipe\\mynamedpipe", //��� ������
			PIPE_ACCESS_DUPLEX | //������ � ������ � �����
			FILE_FLAG_OVERLAPPED, //������� ��������������� ����/�����
			PIPE_TYPE_MESSAGE | // ����� �������� ���������, � �� ����� ����
			PIPE_READMODE_MESSAGE | // ����� ��������� ���������
			PIPE_WAIT, // ������ ����� �������, ����� ����� ������ ��������
			INSTANCES, // ������������ ���������� ����������� ������
			BUFSIZE * 4, //�������� ������ ������
			BUFSIZE * 4, //������� ������ ������
			PIPE_TIMEOUT, // ����� �������� �������
			NULL); //������� ������
		print_time();
		cout << "[MESSAGE] Initializing Pipe[" << i << "]\n";
		if (Pipe[i].hPipeInst == INVALID_HANDLE_VALUE) {
			print_time();
			cout << "[ERROR] CreateNamedPipe failed with " << GetLastError() << '\n';
			return 0;
		}
		//���������� ����� � �������
		Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
		//�������, ��������� �� ����������� � �������
		Pipe[i].dwState = Pipe[i].fPendingIO ?
			CONNECTING_STATE : // ���
			READING_STATE; //��
	}

	while (1) {
		int j = 1;
		//����, ���� ���� �� ������� �������� ���� �� ��������
		dwWait = WaitForMultipleObjects(
			INSTANCES, // ���������� �������
			hEvents, // �������
			false, // �� ����� ����
			INFINITE); // ����� ����������
			//dwWait ���������, ����� ����� �������� ��������. WAIT_OBJECT_0 -		������ ������� ������� �������
		i = dwWait - WAIT_OBJECT_0; //��������� ����� ����� � ������� ��������		��������
		if (i < 0 || i >(INSTANCES - 1)) {
			print_time();
			cout << "[ERROR] Index out of range.\n";
			return 0;
		}

		if (Pipe[i].fPendingIO) {
			fSuccess = GetOverlappedResult(
				Pipe[i].hPipeInst, //��������� ������
				&Pipe[i].oOverlap, //��������� OVERLAPPED ���������������� ������
				&cbRet, //���������� ���������� ������
				false); //�� �����
			switch (Pipe[i].dwState) {
				//���� ������ �������� ������
			case CONNECTING_STATE:
				print_time();
				cout << "[MESSAGE] Connected\n";
				if (!fSuccess) {
					print_time();
					cout << "[ERROR] Error code" << GetLastError() << '\n';
					return 0;
				}
				Pipe[i].dwState = READING_STATE;
				break;
				//���� ������ �������� ������
			case READING_STATE:
				if (!fSuccess || cbRet == 0) {
					DisconnectAndReconnect(i);
					continue;
				}
				Pipe[i].cbRead = cbRet;
				Pipe[i].dwState = WRITING_STATE;
				break;
				//���� ������ �������� ������
			case WRITING_STATE:
				if (!fSuccess || cbRet != 4) {
					DisconnectAndReconnect(i);
					continue;
				}
				Pipe[i].dwState = READING_STATE;
				break;
			default: {
				print_time();
				cout << "[ERROR] Invalid pipe state.\n";
				return 0;
			}
			}
		}
		//��������� ������� ��������
		switch (Pipe[i].dwState) {
			//������ ������ �� ������
		case READING_STATE:
			print_time();
			cout << "[MESSAGE] [" << i << "]:Read\n";
			fSuccess = ReadFile(
				Pipe[i].hPipeInst,
				&Pipe[i].size,
				4,
				&Pipe[i].cbRead,
				&Pipe[i].oOverlap);
			//��� �������� ���������� ������
			if (fSuccess && Pipe[i].cbRead != 0) {
				Pipe[i].fPendingIO = false;
				Pipe[i].dwState = WRITING_STATE;
				continue;
			}
			//���� �������� ������ ��� �� �����������, �� ����������
			dwErr = GetLastError();
			if (!fSuccess && (dwErr == ERROR_IO_PENDING)) {
				Pipe[i].fPendingIO = true;
				continue;
			}
			//����� ���� ��������� ������, ����������� �� �������
			DisconnectAndReconnect(i);
			break;
			//������ ������ � �����
		case WRITING_STATE: {
			if (Pipe[i].size > BUFSIZE) {
				print_time();
				cout << "[ERROR] The numbers size exceeds the size of the buffer\n";
				dwErr = -1;
				fSuccess = WriteFile(Pipe[i].hPipeInst, &dwErr, 4, &cbRet,
					&Pipe[i].oOverlap);
				break;
			}
			print_time();
			cout << "[MESSAGE] [" << i << "]:Write\n";
			for (; j <= Pipe[i].size; j++) {
				Sleep(500);
				fSuccess = WriteFile(Pipe[i].hPipeInst, &j, 4, &cbRet, &Pipe[i].oOverlap);
				if (!fSuccess && pSuccess) {
					DisconnectAndReconnect(i);
					pSuccess = fSuccess;
				}
			}
			int b = -1;
			fSuccess = WriteFile(Pipe[i].hPipeInst, &b, 4, &cbRet, &Pipe[i].oOverlap);
			//��� �������� ���������� ������
			if (fSuccess && cbRet == 4) {
				Pipe[i].fPendingIO = false;
				Pipe[i].dwState = READING_STATE;
				pSuccess = true;
				j = 1;
				continue;
			}
			//���� ������ ��� �� �����������, �� ����������
			dwErr = GetLastError();
			if (!fSuccess && (dwErr == ERROR_IO_PENDING)) {
				Pipe[i].fPendingIO = true;
				continue;
			}
			//����� ���� ��������� ������, ����������� �� �������
			DisconnectAndReconnect(i);
			break;
		}
		default: {
			print_time();
			cout << "[ERROR] Invalid pipe state.\n";
			return 0;
		}
		}
	}
	return 0;
}

//����������� �� �������� ������� � ���� ������
void DisconnectAndReconnect(unsigned int i)
{
	//����������� �� ��������
	//print_time();
	//cout << "[MESSAGE] [" << i << "]:Disconnecting\n";
	if (!DisconnectNamedPipe(Pipe[i].hPipeInst)) {
		print_time();
		cout << "[ERROR] DisconnectNamedPipe failed with " << GetLastError() << '\n';
	}
	//������������ � ������ �������
	Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
	//��������� ���������
	Pipe[i].dwState = Pipe[i].fPendingIO ?
		CONNECTING_STATE : //���� �����������
		READING_STATE; //���������
}

//���� ������ �����������, �� fPendingIO = false
bool ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = false;
	//�������� ������������ � ������
	fConnected = ConnectNamedPipe(hPipe, lpo);
	if (fConnected) {
		print_time();
		cout << "[ERROR] ConnectNamedPipe failed with " << GetLastError() << '\n';
		return 0;
	}
	switch (GetLastError())
	{
		//����������� � ��������. ������� ���������, � ���� ����������� �� ���������, ������������� ������ ������
	case ERROR_IO_PENDING:
		//print_time();
		//cout << "[MESSAGE] Waiting\n";
		fPendingIO = true;
		break;
		//���� ������ ��� ������ �����������. ������ ��������� ������� � ������
	case ERROR_PIPE_CONNECTED:
		print_time();
		cout << "[MESSAGE] Connected\n";
		if (SetEvent(lpo->hEvent))
			break;
		//������
	default: {
		print_time();
		cout << "[ERROR] ConnectNamedPipe failed with " << GetLastError() << '\n';
		return 0;
	}
	}
	return fPendingIO;
}
