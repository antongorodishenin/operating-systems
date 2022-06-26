#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include "..\..\employee.h"

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
LPWSTR stringToLPWSTR(std::string str) {
    wchar_t temp[256];
    mbstowcs(temp, str.c_str(), str.length());
    LPWSTR data = temp;
    return data;
}
int main(int argc, char* argv[])
{
    std::string filename_bin;
    std::string records;

    std::cout << "Binary file: ";
    std::cin >> filename_bin;
    std::cout << "Number of records: ";
    std::cin >> records;

    std::string path = argv[0];
    replace(path, "Main.exe", "Creator.exe");
    std::string cre = path + " " + filename_bin + " " + records;
    STARTUPINFO si;
    PROCESS_INFORMATION piApp;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    if (CreateProcess(NULL, stringToLPWSTR(cre),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &piApp))
    {
        WaitForSingleObject(piApp.hProcess, INFINITE);
        CloseHandle(piApp.hThread);
        CloseHandle(piApp.hProcess);
    }
    else {
        std::cout << "Process Creator.exe not start";
        return 1;
    }


    std::string filepath = path;
    replace(filepath, "Debug\\Creator.exe", "Main\\");
    std::fstream fbin;
    fbin.open(filepath + filename_bin, std::ios::in | std::ios::binary);
    employee emp;
    while (!fbin.eof()) {
        fbin.read((char*)&emp, sizeof(emp));
        if (fbin.eof()) break;
        std::cout << emp.num << " " << emp.name << " " << emp.hours << "\n";
    }

    fbin.close();

    std::string filename_txt;
    std::string salary;

    std::cout << "\nReport file (txt): ";
    std::cin >> filename_txt;
    std::cout << "Salary: ";
    std::cin >> salary;

    replace(path, "Creator.exe", "Reporter.exe");
    std::cout << std::endl;
    std::string rep = path + " " + filename_bin + " " + filename_txt + " " + salary;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    if (CreateProcess(NULL, stringToLPWSTR(rep),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &piApp))
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
    file_txt.open(filepath + filename_txt);
    std::string line;
    if (file_txt.is_open()) {
        while (std::getline(file_txt, line))
        {
            std::cout << line << std::endl;
        }
    }

    file_txt.close();
}
