#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <langinfo.h>
#include <errno.h>
#include <regex>

using namespace std;

/*
������� ����������� ������
- �������� ����������� �����
- �������� ����� �����
- �������� ����������
*/
int copyFile(string filename, string newFilename)
{
    // ������
    while (filename == newFilename)
    {
        cout << " > �������� ����������� ����� � �������� ����� �� ������ ���������" << endl;
        return -1;
    }

    int res = 0;
    ifstream fin;
    // size_t Unsigned integral type - ����������� �����
    size_t bufsize = 100;
    char *buf = new char[bufsize];
    fin.open(filename, ios::binary); // ios::binary ����� ��� ����, ����� ������ ����������� ��� ����, ��� ������-���� ��������������.
    if (fin.is_open())
    {
        ofstream fout;
        fout.open(newFilename, ios::binary);
        if (fout.is_open())
        {
            auto bufCount = 0;
            while (!fin.eof())
            {
                // ������ ������� �� 100 ��������
                fin.read(buf, bufsize);
                bufCount = fin.gcount(); // ���������� ����� ��������, ��������� �� ����� ���������� ������������������ �����.
                if (bufCount)
                    fout.write(buf, bufCount);
            }
            cout << " > ����������� ������ �������!" << endl;
            fout.close();
        }
        else
        {
            cout << " > ���������� ������� ������� ����: " + newFilename << endl;
            res = -1;
        }
        fin.close();
    }
    else
    {
        cout << " > ���������� ������� �������� ����: " + filename << endl;
        res = -1;
    }
    delete[] buf;
    return res;
}

/*
������� ����������� ������
- �������� ��������� �����
- ���� �� ������ �����
- ��������� ����
*/
int moveFile(string fileNameIn, string fileNameOut)
{
    const char *oldfilename = fileNameIn.c_str();
    const char *newfilename = fileNameOut.c_str();
    if (rename(oldfilename, newfilename) == 0) {
        cout << " > ����������� ������ �������!" << endl;
        return 0;
    }
    else {
        cout << " > ������ ����������� �����!" << endl;
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
������� ��������� ���������� � ����� (�����, ������, ����� ���������)
- �������� ��������� �����
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

        cout << "����: " << fileName << endl
             << "����������  �����: " << s_perms(statbuf.st_mode) << endl
             << "������ ����� (����): " << statbuf.st_size << endl
             << "��������� ����� ���������: " << datestring << endl;
        return 0;
    }
    else
        return -1;
}

/*
������� ��������� ���� �� ��������� ����.
- �������� ��������� �����
- ���������� ����������� ���� 777
*/
int changeRights(string fileName, string modeStr)
{
    //��������� ���� �������:
    const char *charFilename = fileName.c_str();

    // �������� ���������� ����
    if (! regex_match(modeStr, regex("[0-7][0-7][0-7]") )) {
        cout << "����� ��� ���� ������� � �������. ������ ���� 3 ����� �� 0 �� 7: " << modeStr << endl;
        return -1;
    }

    mode_t mode = (mode_t) strtol(modeStr.c_str(), NULL, 8);
    
    int res = chmod(charFilename, mode);
    if (res == 0) {
        cout << " > ����� ���� ������ �������!" << endl;
        return 0;
    } else {
        string err = strerror(errno);
        cout << " > ������ ��� ����� ���� � ����� : " << fileName << " (" << modeStr << ") : " << err << endl;
        return -1;
    }
    
}

/*����� ��������� ������ ����� help ��� ������ � ���, �� ������
���������� ��� ������� ��������� � ������ --help.*/

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        cout << "����� �������� - ��� ��������" << endl;
        cout << "��. --help ��� ��������� �������." << endl;
        return -1;
    }

    string op = argv[1];

    // -c copy
    if (op == "-c")
    {
        if (argc < 3)
        {
            cout << "��� ��������� -c ����� ��� ��������� - ��� �������� � ��������� �����."<< endl;
            cout << "��. --help ��� ��������� �������."<< endl;
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
            cout << "��� ��������� -m ����� ��� ��������� - ��� �������� � ��������� �����."<< endl;
            cout << "��. --help ��� ��������� �������."<< endl;
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
            cout << "��� ��������� -s ����� ������ �������� - ��� �����." << endl;
            cout << "��. --help ��� ��������� �������." << endl;
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
            cout << "��� ��������� -r ����� ��� ��������� - ��� ����� � �����=�����." << endl;
            cout << "��. --help ��� ��������� �������." << endl;
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
        cout << endl << "�������" << endl << endl;
        cout << "�������� -c - ����������� �����. ����� ��� ��������� - ��� �������� � ��������� �����."<< endl;
        cout << "�������� -m - ����������� �����. ����� ��� ��������� - ������������� ��� ���������� ���� �������� � ��������� �����."<< endl;
        cout << "�������� -s - ��������� ���������� � ����� (�����, ������, ����� ���������). ����� ������ �������� - ��� �����." << endl;
        cout << "�������� -r - ��������� ���� �� ����. ����� ��� ��������� - ��� ����� � �����=�����." << endl;
        
        return 0;
    
    }
    else

    // ���������
    {
        string s = argv[1];
        cout << "����������� ��������: " + s << endl;
        cout << "��. --help ��� ��������� �������." << endl;
    }
}
