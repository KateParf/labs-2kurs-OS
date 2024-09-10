#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <langinfo.h>
#include <errno.h>
#include <regex>

using namespace std;

/*
Функция копирования файлов
- название копируемого файла
- название копии файла
- копирует содержимое
*/
int copyFile(string filename, string newFilename)
{
    // ошибка
    while (filename == newFilename)
    {
        cout << " > Название копируемого файла и название копии не должны совпадать" << endl;
        return -1;
    }

    int res = 0;
    ifstream fin;
    // size_t Unsigned integral type - беззнаковое целое
    size_t bufsize = 100;
    char *buf = new char[bufsize];
    fin.open(filename, ios::binary); // ios::binary нужен для того, чтобы данные сохранялись как есть, без какого-либо форматирования.
    if (fin.is_open())
    {
        ofstream fout;
        fout.open(newFilename, ios::binary);
        if (fout.is_open())
        {
            auto bufCount = 0;
            while (!fin.eof())
            {
                // читаем блоками по 100 символов
                fin.read(buf, bufsize);
                bufCount = fin.gcount(); // Возвращает число символов, считанных во время последнего неформатированного ввода.
                if (bufCount)
                    fout.write(buf, bufCount);
            }
            cout << " > Копирование прошло успешно!" << endl;
            fout.close();
        }
        else
        {
            cout << " > Невозможно открыть целевой файл: " + newFilename << endl;
            res = -1;
        }
        fin.close();
    }
    else
    {
        cout << " > Невозможно открыть исходный файл: " + filename << endl;
        res = -1;
    }
    delete[] buf;
    return res;
}

/*
Функция перемещения файлов
- название исходного файла
- путь до нового места
- переносит файл
*/
int moveFile(string fileNameIn, string fileNameOut)
{
    const char *oldfilename = fileNameIn.c_str();
    const char *newfilename = fileNameOut.c_str();
    if (rename(oldfilename, newfilename) == 0) {
        cout << " > Перемещение прошло успешно!" << endl;
        return 0;
    }
    else {
        cout << " > Ошибка перемещения файла!" << endl;
        return -1;
    }
}

string s_perms(mode_t perms)
{
    string s = "";
    s += ((perms & S_IRUSR) ? "r" : "-");
    s += ((perms & S_IWUSR) ? "w" : "-");
    s += ((perms & S_IXUSR) ? "x" : "-");
    s += " ";
    s += ((perms & S_IRGRP) ? "r" : "-");
    s += ((perms & S_IWGRP) ? "w" : "-");
    s += ((perms & S_IXGRP) ? "x" : "-");
    s += " ";
    s += ((perms & S_IROTH) ? "r" : "-");
    s += ((perms & S_IWOTH) ? "w" : "-");
    s += ((perms & S_IXOTH) ? "x" : "-");
    return s;
}

/*
Функция получения информации о файле (права, размер, время изменения)
- название исходного файла
*/
int infoFile(string fileName)
{
    const char *charFilename = fileName.c_str();
    struct stat statbuf;
    int stats = stat(charFilename, &statbuf);

    if (stats != -1)
    {
        struct tm *tm;
        char datestring[256];
        tm = localtime(&statbuf.st_mtime);
        strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

        cout << "Файл: " << fileName << endl
             << "Разрешение  файла: " << s_perms(statbuf.st_mode) << endl
             << "Размер файла (байт): " << statbuf.st_size << endl
             << "Последнее время изменения: " << datestring << endl;
        return 0;
    }
    else
        return -1;
}

/*
Функция изменения прав на выбранный файл.
- название исходного файла
- символьное обозначение прав 777
*/
int changeRights(string fileName, string modeStr)
{
    //Изменение прав доступа:
    const char *charFilename = fileName.c_str();

    // проверка валидности моде
    if (! regex_match(modeStr, regex("[0-7][0-7][0-7]") )) {
        cout << "Маска для прав указана с ошибкой. Должно быть 3 числа от 0 до 7: " << modeStr << endl;
        return -1;
    }

    mode_t mode = (mode_t) strtol(modeStr.c_str(), NULL, 8);
    
    int res = chmod(charFilename, mode);
    if (res == 0) {
        cout << " > Смена прав прошла успешно!" << endl;
        return 0;
    } else {
        string err = strerror(errno);
        cout << " > Ошибка при смене прав у файла : " << fileName << " (" << modeStr << ") : " << err << endl;
        return -1;
    }
    
}

/*Также программа должна иметь help для работы с ней, он должен
вызываться при запуске программы с ключом --help.*/

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        cout << "Нужен аргумент - тип операции" << endl;
        cout << "см. --help для получения справки." << endl;
        return -1;
    }

    string op = argv[1];

    // -c copy
    if (op == "-c")
    {
        if (argc < 3)
        {
            cout << "Для параметра -c нужны два параметра - имя входного и выходного файла."<< endl;
            cout << "см. --help для получения справки."<< endl;
            return -1;
        }

        string fileName1 = argv[2];
        string fileName2 = argv[3];
        return copyFile(fileName1, fileName2);
    }
    else

    // -m move
    if (op == "-m")
    {
        if (argc < 3)
        {
            cout << "Для параметра -m нужны два параметра - имя входного и выходного файла."<< endl;
            cout << "см. --help для получения справки."<< endl;
            return -1;
        }

        string fileName1 = argv[2];
        string fileName2 = argv[3];
        return moveFile(fileName1, fileName2);
    }
    else

    // -s - stat
    if (op == "-s")
    {
        if (argc < 3)
        {
            cout << "Для параметра -s нужен третий параметр - имя файла." << endl;
            cout << "см. --help для получения справки." << endl;
            return -1;
        }

        string fileName = argv[2];
        return infoFile(fileName);
    }
    else

    // -r rights
    if (op == "-r")
    {
        if (argc < 3)
        {
            cout << "Для параметра -r нужны два параметра - имя файла и число=права." << endl;
            cout << "см. --help для получения справки." << endl;
            return -1;
        }

        string fileName1 = argv[2];
        string mode = argv[3];
        return changeRights(fileName1, mode);
    }
    else

    //--help
    if (op == "--help")
    {   
        cout << endl << "СПРАВКА" << endl << endl;
        cout << "Параметр -c - копирование файла. Нужны два параметра - имя входного и выходного файла."<< endl;
        cout << "Параметр -m - перемещение файла. Нужны два параметра - относительный или абсолютный путь входного и выходного файла."<< endl;
        cout << "Параметр -s - получение информации о файле (права, размер, время изменения). Нужен третий параметр - имя файла." << endl;
        cout << "Параметр -r - изменение прав на файл. Нужны два параметра - имя файла и число=права." << endl;
        
        return 0;
    
    }
    else

    // непонятно
    {
        string s = argv[1];
        cout << "Неизвестный параметр: " + s << endl;
        cout << "см. --help для получения справки." << endl;
    }
}
