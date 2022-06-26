#include <windows.h>
#include <iostream>
#include <fstream>

#pragma warning(disable : 4996)

HANDLE isFull;
HANDLE isEmpty;
HANDLE mutex;
int messagesNumber;


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
};

void write(char* filename, char* message, char* authorName) {
	WaitForSingleObject(isEmpty, INFINITE);
	WaitForSingleObject(mutex, INFINITE);

	std::fstream f(filename, std::ios::binary | std::ios::in | std::ios::out);

	int messageWritePos;
	char p[10];
	f.read(p, sizeof(p));
	messageWritePos = atoi(p);
	std::cout << "Message write pos: " << messageWritePos << std::endl;

	Message* m = new Message(authorName, message);
	int pos = sizeof(p) + sizeof(Message) * messageWritePos;
	f.seekp(pos, std::ios::beg);
	std::cout << (char*)m << std::endl;
	f.write((char*)m, sizeof(Message));

	messageWritePos++;
	if (messageWritePos == messagesNumber) {
		messageWritePos = 0;
	}
	itoa(messageWritePos, p, 10);
	f.seekp(0, std::ios::beg);
	f.write(p, sizeof(p));

	f.close();

	ReleaseMutex(mutex);
	ReleaseSemaphore(isFull, 1, NULL);
}

int main(int argc, char* argv[]) {
	isFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, charArrayToLPCWSTR("Full"));
	isEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, charArrayToLPCWSTR("Empty"));
	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, charArrayToLPCWSTR("Mutex"));
	char* filename = argv[1];
	messagesNumber = atoi(argv[2]);
	std::cout << "Enter name:\n";
	char name[20];
	std::cin >> name;

	bool doCycle = true;
	while (doCycle) {
		std::cout << "Enter:\n1) Write\n2) Exit\n";
		int answer;
		std::cin >> answer;

		if (answer == 1) {
			std::cout << "Enter message:\n";
			char message[20];
			std::cin >> message;
			write(filename, message, name);
		}
		else if (answer == 2) {
			doCycle = false;
		}
	}
}
