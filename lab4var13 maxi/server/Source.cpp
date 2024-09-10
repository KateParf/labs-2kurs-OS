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
	OVERLAPPED oOverlap; //Структура, позволяющая производить асинхронные операции
	HANDLE hPipeInst; //Дескриптор канала
	DWORD cbRead; //Хранит кол-во прочитанных байт
	unsigned int size;
	unsigned int dwState; //Состояние работы канала: запись, чтение или подключение
	bool fPendingIO; //Состояние, отображающее, подключен ли к каналу клиент
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
	//Инициализация всех экземляров канала
	for (i = 0; i < INSTANCES; i++) {
		//Событие для экземпляра
		hEvents[i] = CreateEvent(
			NULL, //Атрибут защиты
			true, //Ручное управление
			true, //Начальное состояние - активно
			NULL); //Имя
		if (hEvents[i] == NULL) {
			print_time();
			cout << "[ERROR] CreateEvent failed with " << GetLastError() << '\n';
			return 0;
		}
		//Присваиваем структуре OVERLAP событие
		Pipe[i].oOverlap.hEvent = hEvents[i];
		//Инициализируем сам канал
		Pipe[i].hPipeInst = CreateNamedPipe(
			L"\\\\.\\pipe\\mynamedpipe", //Имя канала
			PIPE_ACCESS_DUPLEX | //Чтение и запись в канал
			FILE_FLAG_OVERLAPPED, //Включен перекрывающийся ввод/вывод
			PIPE_TYPE_MESSAGE | // Канал передает сообщения, а не поток байт
			PIPE_READMODE_MESSAGE | // Канал считывает сообщения
			PIPE_WAIT, // Клиент будет ожидать, когда канал станет доступен
			INSTANCES, // Максимальное количество экземпляров канала
			BUFSIZE * 4, //Выходной размер буфера
			BUFSIZE * 4, //Входной размер буфера
			PIPE_TIMEOUT, // Время ожидания клиента
			NULL); //Атрибут защиты
		print_time();
		cout << "[MESSAGE] Initializing Pipe[" << i << "]\n";
		if (Pipe[i].hPipeInst == INVALID_HANDLE_VALUE) {
			print_time();
			cout << "[ERROR] CreateNamedPipe failed with " << GetLastError() << '\n';
			return 0;
		}
		//Подключаем канал к клиенту
		Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
		//Смотрим, произошло ли подключение к клиенту
		Pipe[i].dwState = Pipe[i].fPendingIO ?
			CONNECTING_STATE : // Нет
			READING_STATE; //Да
	}

	while (1) {
		int j = 1;
		//Ждем, пока один из каналов закончит одну из операций
		dwWait = WaitForMultipleObjects(
			INSTANCES, // Количество событий
			hEvents, // События
			false, // Не ждать всех
			INFINITE); // Ждать бесконечно
			//dwWait указывает, какой канал завершил операцию. WAIT_OBJECT_0 -		первый элемент массива каналов
		i = dwWait - WAIT_OBJECT_0; //Указывает какой канал в массиве завершил		операцию
		if (i < 0 || i >(INSTANCES - 1)) {
			print_time();
			cout << "[ERROR] Index out of range.\n";
			return 0;
		}

		if (Pipe[i].fPendingIO) {
			fSuccess = GetOverlappedResult(
				Pipe[i].hPipeInst, //Экземпляр канала
				&Pipe[i].oOverlap, //Структура OVERLAPPED соответствующего канала
				&cbRet, //Количество переданных байтов
				false); //Не ждать
			switch (Pipe[i].dwState) {
				//Ждет начала операции чтения
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
				//Ждет начала операции записи
			case READING_STATE:
				if (!fSuccess || cbRet == 0) {
					DisconnectAndReconnect(i);
					continue;
				}
				Pipe[i].cbRead = cbRet;
				Pipe[i].dwState = WRITING_STATE;
				break;
				//Ждет начала операции чтения
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
		//Выполняем текущее действие
		switch (Pipe[i].dwState) {
			//Читаем данные из канала
		case READING_STATE:
			print_time();
			cout << "[MESSAGE] [" << i << "]:Read\n";
			fSuccess = ReadFile(
				Pipe[i].hPipeInst,
				&Pipe[i].size,
				4,
				&Pipe[i].cbRead,
				&Pipe[i].oOverlap);
			//При успешном завершении чтения
			if (fSuccess && Pipe[i].cbRead != 0) {
				Pipe[i].fPendingIO = false;
				Pipe[i].dwState = WRITING_STATE;
				continue;
			}
			//Если операция чтения еще не закончилась, то пропускаем
			dwErr = GetLastError();
			if (!fSuccess && (dwErr == ERROR_IO_PENDING)) {
				Pipe[i].fPendingIO = true;
				continue;
			}
			//Иначе если произошла ошибка, отключаемся от клиента
			DisconnectAndReconnect(i);
			break;
			//Запись данных в канал
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
			//При успешном завершении записи
			if (fSuccess && cbRet == 4) {
				Pipe[i].fPendingIO = false;
				Pipe[i].dwState = READING_STATE;
				pSuccess = true;
				j = 1;
				continue;
			}
			//Если запись еще не завершилась, то пропускаем
			dwErr = GetLastError();
			if (!fSuccess && (dwErr == ERROR_IO_PENDING)) {
				Pipe[i].fPendingIO = true;
				continue;
			}
			//Иначе если произошла ошибка, отключаемся от клиента
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

//Отключаемся от текущего клиента и ждем нового
void DisconnectAndReconnect(unsigned int i)
{
	//Отключаемся от текущего
	//print_time();
	//cout << "[MESSAGE] [" << i << "]:Disconnecting\n";
	if (!DisconnectNamedPipe(Pipe[i].hPipeInst)) {
		print_time();
		cout << "[ERROR] DisconnectNamedPipe failed with " << GetLastError() << '\n';
	}
	//Подключаемся к новому клиенту
	Pipe[i].fPendingIO = ConnectToNewClient(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
	//Проверяем состояние
	Pipe[i].dwState = Pipe[i].fPendingIO ?
		CONNECTING_STATE : //Ждем подключения
		READING_STATE; //Подключен
}

//Если клиент подключился, то fPendingIO = false
bool ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = false;
	//Пытаемся подключиться к каналу
	fConnected = ConnectNamedPipe(hPipe, lpo);
	if (fConnected) {
		print_time();
		cout << "[ERROR] ConnectNamedPipe failed with " << GetLastError() << '\n';
		return 0;
	}
	switch (GetLastError())
	{
		//Подключение в процессе. Функцию выполнили, и если подключения не произошло, выбрасывается данная ошибка
	case ERROR_IO_PENDING:
		//print_time();
		//cout << "[MESSAGE] Waiting\n";
		fPendingIO = true;
		break;
		//Если клиент уже ожидал подключения. Просто обновляем событие у канала
	case ERROR_PIPE_CONNECTED:
		print_time();
		cout << "[MESSAGE] Connected\n";
		if (SetEvent(lpo->hEvent))
			break;
		//Ошибка
	default: {
		print_time();
		cout << "[ERROR] ConnectNamedPipe failed with " << GetLastError() << '\n';
		return 0;
	}
	}
	return fPendingIO;
}
