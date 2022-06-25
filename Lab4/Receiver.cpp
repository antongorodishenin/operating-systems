#include <Windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>

#pragma warning(disable : 4996)

using namespace std;

HANDLE isFull, isEmpty, mutex;
int messageReadPos = 0;
int messagesNumber;

struct Message {
	char name[10];
	char text[20];

	Message() {
		strcpy(name, "111");
		strcpy(text, "111");
	}

	Message(char* name1, char* text1) {
		strcpy(name, name1);
		strcpy(text, text1);
	}

	Message(const char* name1, const char* text1) {
		strcpy(name, name1);
		strcpy(text, text1);
	}
};

void replace(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	std::string::size_type start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

LPWSTR charArrayToLPWSTR(const char* com) {
	wchar_t temp[250];
	size_t outSize;
	mbstowcs_s(&outSize, temp, com, sizeof(com));
	LPWSTR command = temp;
	return command;
}

LPCWSTR charArrayToLPCWSTR(const char* com) {
	wchar_t temp[250];
	size_t outSize;
	mbstowcs_s(&outSize, temp, com, sizeof(com));
	LPCWSTR command = temp;
	return command;
}

LPWSTR stringToLPWSTR(string com) {
	wchar_t temp[250];
	size_t outSize;
	mbstowcs_s(&outSize, temp, com.c_str(), com.length());
	LPWSTR command = temp;
	return command;
}

LPSTR stringToLPSTR(string com) {
	char temp[250];
	strcpy(temp, com.c_str());
	LPSTR command = temp;
	cout << temp << endl;
	return command;
}

void createFile(string filename, int messagesNumber) {
	ofstream fout(filename, ios::binary);
	int pos = 0;
	char p[10];
	itoa(pos, p, 10);
	fout.write(p, sizeof(p));
	Message* m = new Message("empty", "empty");
	for (int i = 0; i < messagesNumber; i++) {
		fout.write((char*)&m, sizeof(Message));
	}
	fout.close();
}

void read(string filename) {
	WaitForSingleObject(isFull, INFINITE);
	WaitForSingleObject(mutex, INFINITE);
	cout << "Message read pos: " << messageReadPos << endl;

	fstream f(filename, ios::binary | ios::in | ios::out);
	if (!f.is_open()) {
		cout << "error\n";
		return;
	}

	Message* m = new Message();
	char writeIter[10];
	int pos = sizeof(writeIter) + 30 * messageReadPos;

	f.seekg(pos, ios::beg);
	f.read((char*)m, sizeof(Message));
	cout << "Author: " << m->name << ", text: " << m->text << endl;
	f.seekp(pos, ios::beg);
	m = new Message("empty", "empty");
	f.write((char*)m, sizeof(Message));

	messageReadPos++;
	if (messageReadPos == messagesNumber) {
		messageReadPos = 0;
	}

	f.close();

	ReleaseMutex(mutex);
	ReleaseSemaphore(isEmpty, 1, NULL);
}

void main(int argc, char* argv[]) {
	cout << "Enter file name:\n";
	string filename;
	cin >> filename;

	cout << "Enter messages number:\n";
	cin >> messagesNumber;
	createFile(filename, messagesNumber);

	cout << "Enter Sender process count: \n";
	int senderProcessCount;
	cin >> senderProcessCount;

	isFull = CreateSemaphore(NULL, 0, messagesNumber, charArrayToLPCWSTR("Full"));
	isEmpty = CreateSemaphore(NULL, messagesNumber, messagesNumber, charArrayToLPCWSTR("Empty"));
	mutex = CreateMutex(NULL, FALSE, charArrayToLPCWSTR("Mutex"));

	STARTUPINFO* si = new STARTUPINFO[senderProcessCount];
	PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[senderProcessCount];

	char num[10];
	itoa(messagesNumber, num, 10);
	string path = argv[0];
	replace(path, "LR_4.exe", "Sender.exe ");
	string data = path + filename + " " + num;

	for (int i = 0; i < senderProcessCount; i++) {
		ZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		cout << data << endl;
		if (!CreateProcess(NULL, stringToLPWSTR(data), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
			cout << "The new process is not created.\n";
			return;
		}
	}

	system("cls");
	bool doCycle = true;
	while (doCycle) {
		cout << "Enter:\n1) Read\n2) Exit\n";
		int answer;
		cin >> answer;

		if (answer == 1) {
			read(filename);
		}
		else doCycle = false;
	}

}
