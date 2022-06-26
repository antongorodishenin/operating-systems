#include <Windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>

#pragma warning(disable : 4996)

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

LPWSTR stringToLPWSTR(std::string com) {
	wchar_t temp[250];
	size_t outSize;
	mbstowcs_s(&outSize, temp, com.c_str(), com.length());
	LPWSTR command = temp;
	return command;
}

LPSTR stringToLPSTR(std::string com) {
	char temp[250];
	strcpy(temp, com.c_str());
	LPSTR command = temp;
	std::cout << temp << std::endl;
	return command;
}

void createFile(std::string filename, int messagesNumber) {
	std::ofstream fout(filename, std::ios::binary);
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

void read(std::string filename) {
	WaitForSingleObject(isFull, INFINITE);
	WaitForSingleObject(mutex, INFINITE);
	std::cout << "Message read pos: " << messageReadPos << std::endl;

	std::fstream f(filename, std::ios::binary | std::ios::in | std::ios::out);
	if (!f.is_open()) {
		std::cout << "error\n";
		return;
	}

	Message* m = new Message();
	char writeIter[10];
	int pos = sizeof(writeIter) + 30 * messageReadPos;

	f.seekg(pos, std::ios::beg);
	f.read((char*)m, sizeof(Message));
	std::cout << "Author: " << m->name << ", text: " << m->text << std::endl;
	f.seekp(pos, std::ios::beg);
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
	std::cout << "Enter file name:\n";
	std::string filename;
	std::cin >> filename;

	std::cout << "Enter messages number:\n";
	std::cin >> messagesNumber;
	createFile(filename, messagesNumber);

	std::cout << "Enter Sender process count: \n";
	int senderProcessCount;
	std::cin >> senderProcessCount;

	isFull = CreateSemaphore(NULL, 0, messagesNumber, charArrayToLPCWSTR("Full"));
	isEmpty = CreateSemaphore(NULL, messagesNumber, messagesNumber, charArrayToLPCWSTR("Empty"));
	mutex = CreateMutex(NULL, FALSE, charArrayToLPCWSTR("Mutex"));

	STARTUPINFO* si = new STARTUPINFO[senderProcessCount];
	PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[senderProcessCount];

	char num[10];
	itoa(messagesNumber, num, 10);
	std::string path = argv[0];
	replace(path, "LR_4.exe", "Sender.exe ");
	std::string data = path + filename + " " + num;

	for (int i = 0; i < senderProcessCount; i++) {
		ZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		std::cout << data << std::endl;
		if (!CreateProcess(NULL, stringToLPWSTR(data), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
			std::cout << "The new process is not created.\n";
			return;
		}
	}

	system("cls");
	bool doCycle = true;
	while (doCycle) {
		std::cout << "Enter:\n1) Read\n2) Exit\n";
		int answer;
		std::cin >> answer;

		if (answer == 1) {
			read(filename);
		}
		else doCycle = false;
	}

}
