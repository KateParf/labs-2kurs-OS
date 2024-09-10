#include <iostream>

#include <limits>
using namespace std;

void Program()
{
    cout << "Hello!\n"
         << "This program removes duplicate numbers from an array." << endl;
}

// �������, ������� �� ���������� ������� ��������
void uncorrect()
{
    // ������� ��������� ����� � ��������� ��� ������
    cin.clear();
    // ������� ������ �� ������������� �������� ���� int (streamsize) �� ����, ���� �� ������ enter
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "I don't understand you, sorry. Please, try again.\n";
}

char valid_continue()
{
    cout << "Do you want to continue? (y/n) >> ";
    char answer;
    cin >> answer;
    // � ������� ���������� locale � ������� tolower �������� �������
    answer = tolower(answer);
    while (answer != 'y' && answer != 'n' || cin.peek() != '\n') // ������� peek ��������� ��������� ������ �� ������ ���������� �� ��������� ���
    {
        uncorrect();
        cout << "Do you want to continue? (y/n) >> ";
        cin >> answer;
    }
    return answer;
}

int main() {

    Program();

    char answer;
    
    do {

        int n;
        cout << "Please, enter a array size >> ";
        cin >> n;
        while (cin.fail() || n < 0 || cin.peek() != '\n')
        {
            uncorrect();
            cout << "Enter a positive number >> ";
            cin >> n;
        }
        

        int curSize = 0;
        double* arr = (double*)malloc(curSize * sizeof(double)); // allocate memory 
        double el;
        
        
        for (int i = 0; i < n; i++)
        {            
            cout << "Please, enter a array value >> ";
            cin >> el;
            while (cin.fail() || cin.peek() != '\n')
            {
                uncorrect();
                cout << "Enter a number >> ";
                cin >> el;
            }

            bool flag = false;
            for (int j = 0; j < curSize; j++)
            {
                if (el == arr[j]) {
                    flag = true;
                    break;
                }
            }

            if (!flag){
                curSize ++;
                arr = (double*)realloc(arr, curSize * sizeof(double)); // resize the array
                arr[curSize - 1 ] = el;
            }
            
        }

        cout << "Your array without repetitions << ";
        for (int i = 0; i < curSize; i++)
        {
            cout << arr[i] << " ";
        }
        cout << endl;
        free(arr); // free the memory
        answer = valid_continue();

    } while (answer == 'y');    

    cout << "Thanks for using this program.\n"
         << "Goodbye!";

    return 0;
}

