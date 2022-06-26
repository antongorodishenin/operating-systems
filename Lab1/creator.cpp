#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include "..\..\employee.h"
 

int main(int argc, char* argv[])
{
 
    std::string fileName;
    int size = atoi(argv[2]);
    std::cout << std::endl << size << std::endl;
    fileName = argv[1];
    
 
    std::fstream file_bin;
    file_bin.open(fileName, std::ios::out | std::ios::binary);
    if (file_bin.is_open()) std::cout << argv[1] << " is open" << std::endl;
    else  cout << argv[1] << " isn't open" << std::endl;

    for (int i = 0; i < size; ++i) {
        employee emp;
        std::cout << "Write num: ";
        std::cin >> emp.num;
        std::cout << "Write name: ";
        std::cin >> emp.name;
        std::cout << "Write hours: ";
        std::cin >> emp.hours;

        file_bin.write((char*)&emp, sizeof(emp));
    }
    file_bin.close();
   
    
    return 0;
}
