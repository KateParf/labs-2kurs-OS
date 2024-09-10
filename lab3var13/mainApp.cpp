#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <cstring>
#include <tuple>
//#include <unistd.h>
#include "front.h"
#include "back.h"

using namespace std;

/// #ifdef __linux__ 
/// #ifdef  _WIN32

#ifdef __linux__ 
pid_t pid;
#endif


int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "Russian");

	if (argc == 2 && !strcmp(argv[1], "--help")) {
		help();
		exit(0);
	}

	// ��� ������� - ����� ������ �� �����
	if (argc == 2 && !strcmp(argv[1], "-f")) {
		doBack(false, NULL, NULL);
		exit(0);
	}

	if (argc != 1) {
		cout << "��������� ��������� � ������ --help ��� ��������� �������" << endl;
		exit(1);
	}

	#ifdef __linux__ 
	// ����� ������� � ������ �������� ������ � �������
	cout << "������� ����� ����� �� ������� ..." << endl;
	 
	// ������������� ������ ����� in � out
	int pipe_fr_bk[2]; // ����� ��� �������� ������ �� ������ �� ���
	int pipe_bk_fr[2]; // ����� ��� �������� ������ �� ���� �� �����

	// ������� ������ ��� ������ ����� ������� � �����
	pipe(pipe_fr_bk);
	pipe(pipe_bk_fr);

	// �������
	pid = fork();
	if (pid < 0) {
		cerr << "����������� ������! ���� �� ������" << endl;
		exit(1);
	}

	if (pid > 0) {
		doFront(pipe_fr_bk, pipe_bk_fr);
	}
	else {
		doBack(pipe_fr_bk, pipe_bk_fr);
	}

	// ��������� ������
	for (int i = 0; i < 2; ++i)	{
		close(pipe_fr_bk[i]);
		close(pipe_bk_fr[i]);
	}
	return 0;

	#else
		cout << "����� ����� �� ������� ���������� ������ � linux" << endl;
		return 1;
	#endif

}