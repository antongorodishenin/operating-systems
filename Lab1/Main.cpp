#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include "..\..\employee.h"
using namespace std;
#pragma warning(disable : 4996)
void replace(string& str, const string& from, const string& to) {
    if (from.empty())
        return;
    string::size_type start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
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
{   string filename_bin;
    string records;

    cout << "Binary file: ";
    cin >> filename_bin;
    cout << "Number of records: ";
    cin >> records;

    string path = argv[0];
    replace(path, "Main.exe", "Creator.exe");
    string cre = path + " " + filename_bin + " " + records;
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
     cout<<"Process Creator.exe not start";
     return 1;
    }
 
      
       string filepath = path;
       replace(filepath, "Debug\\Creator.exe", "Main\\");
    fstream fbin;
    fbin.open(filepath+filename_bin, ios::in | ios::binary); 
    employee emp;
    while (!fbin.eof()) {
           fbin.read((char*)&emp, sizeof(emp));
           if (fbin.eof()) break;
           cout<< emp.num << " " << emp.name << " " << emp.hours << "\n";
       }
    
    fbin.close();
   
    std::string filename_txt;
    string salary;

    cout << "\nReport file (txt): ";
    cin >> filename_txt;
    cout << "Salary: ";
    cin >> salary;
    
    replace(path, "Creator.exe", "Reporter.exe");
    cout << endl;
    string rep = path + " " +filename_bin+" "+ filename_txt + " " + salary;
    ZeroMemory(&si, sizeof(STARTUPINFO));
   if (CreateProcess(NULL, stringToLPWSTR(rep),
           NULL, NULL, FALSE, 0, NULL, NULL, &si, &piApp))
    {
        WaitForSingleObject(piApp.hProcess, INFINITE);
        CloseHandle(piApp.hThread);
        CloseHandle(piApp.hProcess);
    }
    else {
        cout << "Process Reporter.exe not start";
        return 1;
    }

    fstream file_txt;
    file_txt.open(filepath + filename_txt);
    string line;
   if (file_txt.is_open()) {
        while (getline(file_txt, line))
        {
            cout << line << endl;
        }
    }
    
    file_txt.close();
   

}
