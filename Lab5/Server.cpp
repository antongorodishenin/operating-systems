#include <iostream>
#include <fstream>
#include <Windows.h>

#pragma warning(disable : 4996)

char READ = 'r';
char MODIFY = 'm';
char ORDER_NOT_FOUND = 'n';
char ORDER_FOUND = 'f';
char EXIT = 'e';
char END_OPERATION = 'd';
char END_MODIFY = 'n';
char filename[20];

volatile int readerCount = 0;
CRITICAL_SECTION cs;
HANDLE semaphore;

struct Order {
	int n;
	char name[10];
	int amount;
	double price;
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

LPWSTR stringToLPWSTR(std::string com) {
	wchar_t temp[250];
	size_t outSize;
	mbstowcs_s(&outSize, temp, com.c_str(), com.length());
	LPWSTR command = temp;
	return command;
}

Order* findOrder(int orderNumber) {
	std::ifstream in(filename, std::ios::binary);
	while (in.peek() != EOF)
	{
		Order* order = new Order;
		in.read((char*)order, sizeof(Order));
		if (order->n == orderNumber) {
			in.close();
			return order;
		}
	}

	in.close();

	return nullptr;
}

void modify(Order order) {
	std::fstream f(filename, std::ios::binary | std::ios::in | std::ios::out);
	int pos = 0;
	int orderSize = sizeof(Order);
	while (f.peek() != EOF)
	{
		Order o;
		f.read((char*)&o, sizeof(Order));
		if (order.n == o.n) {
			f.seekp(pos * orderSize, std::ios::beg);
			f.write((char*)&order, sizeof(Order));
			f.close();
			return;
		}
		else {
			pos++;
		}
	}
}

DWORD WINAPI client(LPVOID data)
{
	HANDLE writePipe;
	HANDLE readPipe;
	HANDLE clientReadPipe;
	HANDLE clientWritePipe;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&readPipe, &clientWritePipe, &sa, 0)) {
		std::cout << "Create pipe failed.\n";
		system("pause");
		return 0;
	}

	if (!CreatePipe(&clientReadPipe, &writePipe, &sa, 0)) {
		std::cout << "Create pipe failed.\n";
		system("pause");
		return 0;
	}

	char comLine[333];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	sprintf(comLine, "C:\\lab5c.exe %d %d", (int)clientWritePipe, (int)clientReadPipe);
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	std::cout << comLine << std::endl;
	system("pause");
	if (!CreateProcess(NULL, stringToLPWSTR(comLine), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		std::cout << "The new process is not created.\n";
		return 0;
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	bool doCycle = true;
	while (doCycle) {
		DWORD bytesRead;
		DWORD bytesWrite;

		char clientWant;
		ReadFile(readPipe, &clientWant, sizeof(clientWant), &bytesRead, NULL);

		if (clientWant == READ) {
			EnterCriticalSection(&cs);
			++readerCount;
			if (readerCount == 1) {
				WaitForSingleObject(semaphore, INFINITE);
			}
			LeaveCriticalSection(&cs);

			int orderNumber;
			ReadFile(readPipe, &orderNumber, sizeof(int), &bytesRead, NULL);
			Order* order = findOrder(orderNumber);

			if (order != nullptr) {
				WriteFile(writePipe, &ORDER_FOUND, sizeof(ORDER_FOUND), &bytesWrite, NULL);
				WriteFile(writePipe, order, sizeof(Order), &bytesWrite, NULL);
			}
			else {
				WriteFile(writePipe, &ORDER_NOT_FOUND, sizeof(ORDER_NOT_FOUND), &bytesWrite, NULL);
			}

			char end;
			ReadFile(readPipe, &end, sizeof(end), &bytesRead, NULL);

			EnterCriticalSection(&cs);
			--readerCount;
			if (readerCount == 0) {
				ReleaseSemaphore(semaphore, 1, NULL);
			}
			LeaveCriticalSection(&cs);
		}
		else if (clientWant == MODIFY) {
			WaitForSingleObject(semaphore, INFINITE);

			Order order;
			ReadFile(readPipe, &order, sizeof(Order), &bytesRead, NULL);
			modify(order);
			WriteFile(writePipe, &END_MODIFY, sizeof(END_MODIFY), &bytesWrite, NULL);

			char end;
			ReadFile(readPipe, &end, sizeof(end), &bytesRead, NULL);

			ReleaseSemaphore(semaphore, 1, NULL);
		}
		else {
			doCycle = false;
		}
	}
	return 0;
}

void createBinaryFile() {
	std::ofstream out(filename, std::ios::binary);

	std::cout << std::endl << "How many orders you want to write?\n";
	int num;
	std::cin >> num;

	for (int i = 0; i < num; i++)
	{
		system("cls");
		Order order;
		std::cout << "Enter order number:\n";
		std::cin >> order.n;

		std::cout << "Enter order name:\n";
		std::cin >> order.name;

		std::cout << "Enter product count:\n";
		std::cin >> order.amount;

		std::cout << "Enter product price:\n";
		std::cin >> order.price;

		out.write((char*)&order, sizeof(struct Order));
	}

	out.close();
}

void checkDataInBinaryFile()
{
	std::ifstream in(filename, std::ios::binary);
	system("cls");
	std::cout << "Orders:\n";
	while (in.peek() != EOF)
	{
		Order order;
		in.read((char*)&order, sizeof(Order));
		std::cout << "Order number:\n" << order.n << "\nOrder name:\n" << order.name << "\nProduct count:\n" << order.amount << "\nProduct price:\n" << order.price << std::endl << std::endl;
	}

	in.close();
}

void main(int argc, char* argv[])
{
	InitializeCriticalSection(&cs);
	semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	std::cout << argv[0];
	std::cout << "Enter file order name:\n";
	std::cin >> filename;

	createBinaryFile();
	checkDataInBinaryFile();

	std::cout << "Enter client count:\n";
	int clientCount;
	std::cin >> clientCount;
	system("cls");
	HANDLE* handles = new HANDLE[clientCount];
	DWORD* ID = new DWORD[clientCount];
	for (int i = 0; i < clientCount; i++)
	{
		handles[i] = CreateThread(NULL, NULL, client, (void*)i, 0, &ID[i]);
	}
	WaitForMultipleObjects(clientCount, handles, TRUE, INFINITE);
	system("cls");
	checkDataInBinaryFile();

	system("pause");
}
