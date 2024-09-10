#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <cstring>
#include <cmath>
#include <tuple>

#include "back.h"

using namespace std;

// вывод сообщения об ошибке
// в канал или консоль в зависимости от способа запуска
void error(string error) {
	cout << error << endl;
}

void debug(string msg) {
	cout << msg;
}

//-- matrix 
double** submatrix(double** A, int n, int x, int y) {
	double** submatrix = new double* [n - 1];
	int subi = 0;
	for (int i = 0; i < n; i++) {
		submatrix[subi] = new double[n - 1];
		int subj = 0;
		if (i == y) {
			continue;
		}
		for (int j = 0; j < n; j++) {
			if (j == x) {
				continue;
			}
			submatrix[subi][subj] = A[i][j];
			subj++;
		}
		subi++;
	}
	return submatrix;
}

// определитель матрицы
double matrixDet(int n, double** A) {
	double det = 0;
	if (n == 1)
		return A[0][0];

	if (n == 2) {
		return A[0][0] * A[1][1] - A[1][0] * A[0][1];
	}

	for (int i = 0; i < n; ++i) {
		det += ((i % 2 == 0 ? 1 : -1) * A[0][i] * matrixDet(n - 1, submatrix(A, n, i, 0)));
	}
	return det;
}

// матрица алг дополнений
double** matrixDopol(int n, double** A) {
	double** res = new double* [n];
	for (int i = 0; i < n; ++i)
		res[i] = new double[n];

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) 
			res[i][j] = ((i+j) % 2 == 0 ? 1 : -1) * matrixDet(n-1, submatrix(A, n, j, i));

	return res;
}

// транспонированная матрица (кв)
double** matrixTransp(int n, double** A) {
	double** res = new double* [n];
	for (int i = 0; i < n; ++i)
		res[i] = new double[n];

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			res[i][j] = A[j][i];

	return res;
}

// умножение кв матрицы на вектор
double* matrixMul(int n, double** A, double* b) {	
	double sum = 0;
	double* res = new double [n];

	for (int i = 0; i < n; i++)	{
		res[i] = 0;
		for (int k = 0; k < n; k++)
			res[i] += A[i][k] * b[k];
	}

	return res;
}

// умножение кв матрицы на число
double** matrixMul(int n, double** A, double d) {
	double** res = new double* [n];
	for (int i = 0; i < n; ++i)
		res[i] = new double[n];

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			res[i][j] = A[i][j] * d;

	return res;
}

void matrixPrint(int n, double** A) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			cout << A[i][j] << " ";
		cout << endl;
	}
	cout << endl << endl;
}

/*
//Функция вычисления обратной матрицы
//Принимает размеры и саму матрицу
//Возвращает обратную матрицу
*/
double** matrixInverse(int n, double** A) {
	matrixPrint(n, A);

	double det = matrixDet(n, A);
	cout << det << endl;
	double obrdet;
	if (det != 0)
		obrdet = 1 / det;
	else {
		cout << "Определитель равен нулю. Дальнейшие вычисления невозможны!" << endl;
		return NULL;
	}

	double** Amin = matrixDopol(n, A);
	matrixPrint(n, Amin);
	double** Amintrans = matrixTransp(n, Amin);
	matrixPrint(n, Amintrans);
	
	double** Ainv = matrixMul(n, Amintrans, obrdet);
	matrixPrint(n, Ainv);
	
	// удаляем промежуточные подмассивы
	for (int i = 0; i < n; ++i) {
		delete[] Amin[i];
		delete[] Amintrans[i];
	}
	// удаляем промежуточные массивы 
	delete[] Amin;
	delete[] Amintrans;

	return Ainv;
}


/*
//Функция решения системы линейных алгебраических уравнений матричным методом
//Система в матричной форме: AX = B, где A — основная матрица системы, B и X — столбцы свободных членов и решений системы
//X = A-1 * B
//Принимает
//Возвращает результат вычисления
*/
double* calc(double** A, double* b, int n)
{
	double** revA = matrixInverse(n, A);
	double* x = matrixMul(n, revA, b);

	// удаляем промежуточный подмассивы
	for (int i = 0; i < n; ++i) {
		delete[] revA[i];
	}
	// удаляем промежуточные массивы 
	delete[] revA;

	return x;
}

//---
typedef tuple<int, double**, double*> readDateRes;

const readDateRes tNULL(NULL, NULL, NULL);

// читает входные данные из файла
readDateRes readFile() {

	ifstream myfile;
	myfile.open("in.txt");

	if (!myfile) {
		error("Ошибка открытия файла");
		return tNULL;
	}

	debug("Читаем из файла...");

	int n;
	string el;
	myfile >> el;
	try {
		n = stoi(el);
		if (n <= 0) {
			throw "Отрицательный размер";
		}
	}
	catch (...) {
		error("Введено не целое положительное число в размере матрицы n. Поменяйте значение в файле.");
		return tNULL;
	}

	double** A = new double* [n];
	for (int i = 0; i < n; ++i)
		A[i] = new double[n];
	double* b = new double[n];

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			string el;
			myfile >> el;
			try {
				A[i][j] = stod(el);
			}
			catch (...) {
				error("Введено не число в элементе (" + to_string(i + 1) + "," + to_string(j + 1) + ") матрицы A. Поменяйте значение в файле.");

				//чистим память
				for (int i = 0; i < n; ++i)
					delete[] A[i];
				delete[] A;
				delete[] b;

				return tNULL;
			}
		}
	}

	for (int i = 0; i < n; i++) {
		string el;
		myfile >> el;
		try {
			b[i] = stod(el);
		}
		catch (...) {
			error("Введено не число в элементе " + to_string(i + 1) + " вектора b. Поменяйте значение в файле.");

			//чистим память
			for (int i = 0; i < n; ++i)
				delete[] A[i];
			delete[] A;
			delete[] b;

			return tNULL;
		}
	}

	// все хорошо, возвращаем данные
	readDateRes t(NULL, NULL, NULL);

	get<0>(t) = n;
	get<1>(t) = A;
	get<2>(t) = b;

	myfile.close();
	return t;
}

// читает входные данные из файла
readDateRes readPipe(int pipe_fr_bk[2]) {
	/*
	read int n

	for i = 0 .. n
		for j = 0 .. n
			read double A[i][j]
	for i = 0 .. n
		read double b[i]
	*/
	return tNULL;
}

// записывает результирующие данные
void saveDataFile(int n, double* x) {
	ofstream myfile;
	myfile.open("out.txt");
	if (!myfile) {
		error("Ошибка открытия файла.");
	}
	else {
		debug("Записываем результат в файл.");
		myfile << n << endl;

		for (int i = 0; i < n; ++i)
			myfile << x[i] << " ";
		myfile << endl;
	}
	myfile.close();
}

void saveDataPipe(int n, double* x, int pipe_bk_fr[2]) {
//
}

//---
 
// основная логика бека
void doBack(bool fromPipe, int pipe_fr_bk[2], int pipe_bk_fr[2]) {
	// считать из канала данные
	readDateRes res = (fromPipe) ? readPipe(pipe_fr_bk) : readFile();

	// если были ошибки чтения
	if (res == tNULL) {
		return;
	}

	// достаем данные из результата функции
	int n = get<0>(res);
	double** A = get<1>(res);
	double* b  = get<2>(res);

	// запускаем расчет
	double* x = calc(A, b, n);

	// вывести в канал ответ
	if (fromPipe)
		saveDataPipe(n, x, pipe_bk_fr);
	else
		saveDataFile(n, x);
}
