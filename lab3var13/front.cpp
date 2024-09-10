#ifndef FRONT_H
#define FRONT_H
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
//#include <unistd.h>
#include <cstring>
#include <tuple>

#include "front.h"

using namespace std;


void help()
{
    cout << "Справка:\n"
        << "Чтобы решить систему линейных алгебраических уравнений матричным методом, запустите программу без ключей\n"
        << "В первой строке введите целое положительное число n. Это размер квадратной матрицы A размером n*n \n"
        << "В следующих n строках введите n чисел - коэффициенты n-го уравнения системы\n"
        << "В последней строке введите n чисел - вектор b из свободных членов\n";
}
/*
//Функция чтения чисел с плавающей точкой
//Производит проверку ввода, с случае ошибки ввод повторяется
//Возвращает правильное число, введенное пользователем
*/
double readDouble(const string& msg)
{
    double result;
    bool flag = true;
    while (flag)
    {
        cout << msg;
        cin >> result;
        if ((cin.fail() || (cin.peek() != '\n')))
        {
            cin.clear();
            // очистка буфера по максимальному значению типа int (streamsize) до того, пока не найдет enter
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << " > Ошибка ввода, попробуйте еще раз\n";
        }
        else
        {
            flag = false;
        }
    }
    return result;
}
/*
//Функция чтения чисел
//Производит проверку ввода, с случае ошибки ввод повторяется
//Возвращает правильное число, введенное пользователем
*/
int readInt(const string& msg)
{
    int result;
    bool flag = true;
    while (flag)
    {
        cout << msg;
        cin >> result;
        if ((result <= 0) || (cin.fail() || (cin.peek() != '\n')))
        {
            cin.clear();
            // очистка буфера по максимальному значению типа int (streamsize) до того, пока не найдет enter
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << " > Ошибка ввода, попробуйте еще раз\n";
        }
        else
        {
            flag = false;
        }
    }
    return result;
}

//---
typedef tuple<int, double**, double*> readDateResFront;

// ---- консоль
readDateResFront readDataCons() {

	int n = readInt("Введите целое положительное значение n: ");

	double** A = new double* [n];
	for (int i = 0; i < n; ++i)
		A[i] = new double[n];
	double* b = new double[n];

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			A[i][j] = readDouble("Введите элемент матрицы A: ");
			

	for (int i = 0; i < n; i++)
		b[i] = readDouble("Введите элемент вектора b: ");
	

    readDateResFront t(n, A, b);
	return t;
}

void printData(int n, double* x) {
    cout << "Корни системы уравнений: " << endl;
    for (int i = 0; i < n; i++) {
            cout << x[i] << " ";
    }    
    cout << endl;
}

void doFront(int pipe_fr_bk[2], int pipe_bk_fr[2]) {
    /*
    // --- консоль
    readDateRes inData = readDataCons();

    int n;

    // пишем данные для расчета в канал
    write(pipe_in[1], &data, sizeof(IntegralData));

    // получаем данные из канала
    // сначала приходит код результата: 0 = ок, -1 = ошибка
    // если ок, то дальше приходит массив размерности Н - результаты
    // если не ок то 
    // - 1 число (инт) - длина строки про ошибку
    // потом массив байт - сообщение об ошибке
    double* res = new double[n];
    read(pipe_out[0], &result, sizeof(double));
        
    // выводим
    printData(n, res);

    delete[] res;
    */
}

#endif