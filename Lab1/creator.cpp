#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>
using namespace std;
#include "..\..\employee.h"
 

int main(int argc, char* argv[])
{
 
    string fileName;
    int size = atoi(argv[2]);
    cout << endl << size << endl;
    fileName = argv[1];
    
 
    fstream file_bin;
    file_bin.open(fileName, ios::out | ios::binary);
    if (file_bin.is_open()) cout << argv[1] << " is open" << "\n";
    else  cout << argv[1] << " isn't open" << "\n";

    for (int i = 0; i < size; ++i) {
        employee emp;
        cout << "Write num: ";
        cin >> emp.num;
        cout << "Write name: ";
        cin >> emp.name;
        cout << "Write hours: ";
        cin >> emp.hours;

        file_bin.write((char*)&emp, sizeof(emp));
    }
    file_bin.close();
   
    
    return 0;
}
