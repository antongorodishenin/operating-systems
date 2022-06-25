#include <iostream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "..\..\employee.h"



int main(int argc, char* argv[])
{
    std::string filename_bin; 
        std::string filename_txt;
    double salary = std::atof(argv[3]);
    filename_bin = argv[1];
    filename_txt = argv[2];
   

    std::fstream fbin;
    std::ofstream ftxt;
    ftxt.open(filename_txt);

    if (ftxt.is_open()) std::cout <<"\n"<< argv[2]<<" is open";
    else std::cout <<"\n"<<argv[2] << " isn't open"<<"\n";

    ftxt<<"\n"<<"Report on file '"<<argv[1]<<"'\n";

    fbin.open(filename_bin, std::ios::in | std::ios::binary); 
    if (fbin.is_open()) std::cout <<"\n"<< argv[1] << " is open";
    else std::cout<< "\n" << argv[1] << " isn't open";

    employee emp;
    while (!fbin.eof()){

        fbin.read((char*)&emp, sizeof(emp));
        if (fbin.eof()) break;
        ftxt << emp.num << " " << emp.name << " " << emp.hours << " " << (emp.hours * salary) << "\n";
    } 
   

       
    
    fbin.close();
    ftxt.close();

    return 0;
}
