#include <iostream>
#include <string>
#include<fstream>
#include <windows.h>
#include <conio.h>
#include "..\..\employee.h"
using namespace std;
#pragma warning(disable : 4996)
void replace(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    std::string::size_type start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
LPWSTR stringToLPWSTR(string str) {
    wchar_t temp[256];
    mbstowcs(temp, str.c_str(), str.length());
    LPWSTR data = temp;
    return data;
}
int main(int argc, char* argv[])
{   std::string fileName;
    std::string numberOfRecords;
    setlocale(LC_ALL, "Rus");

    std::cout << "Write name of bin file: ";
    std::cin >> fileName;
    std::cout << "Write number of records: ";
    std::cin >> numberOfRecords;

    std::string path = argv[0];
    replace(path, "Main.exe", "");
    string cre = "Creator " + fileName + " " + numberOfRecords;

    STARTUPINFO si;
    PROCESS_INFORMATION piApp;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
       if (CreateProcess(NULL,stringToLPWSTR(cre),
           NULL,NULL,FALSE,0,NULL,NULL,&si,&piApp))
    {
       WaitForSingleObject(piApp.hProcess, INFINITE);
       CloseHandle(piApp.hThread);
       CloseHandle(piApp.hProcess);
    }
    else {
     std::cout<<"Process Creator.exe not start";
     return 1;
    }
 
      
      

   
    std::fstream file_bin;
    file_bin.open(path+fileName, std::ios::in | std::ios::binary); 
    employee emp;
    while (!file_bin.eof()) {

           file_bin.read((char*)&emp, sizeof(emp));
           if (file_bin.eof()) break;
           std::cout<< emp.num << " " << emp.name << " " << emp.hours << "\n";
       }
    
    file_bin.close();
   
    std::string fileNameTxt;
    double salary;

    std::cout << "\nWrite name of report file: ";
    std::cin >> fileNameTxt;
    std::cout << "Write salary: ";
    std::cin >> salary;
    

    ZeroMemory(&si, sizeof(STARTUPINFO));
   if (CreateProcess((LPCWSTR)(path + "Reporter.exe").c_str(), 
       (LPWSTR)(path + "Reporter.exe " + fileName + " " + fileNameTxt + " " + std::to_string(salary)).c_str(),
       NULL, NULL, FALSE, NULL, NULL, NULL, &si, &piApp))
    {
        WaitForSingleObject(piApp.hProcess, INFINITE);
        CloseHandle(piApp.hThread);
        CloseHandle(piApp.hProcess);
    }
    else {
        std::cout << "Process Reporter.exe not start";
        return 1;
    }

    std::fstream file_txt;
    file_txt.open(path + fileNameTxt);
    std::string line;
   if (file_txt.is_open()) {
        while (std::getline(file_txt, line))
        {
            std::cout << line << std::endl;
        }
    }
    
    file_txt.close();
   

}

