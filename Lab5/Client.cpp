#include <windows.h>
#include <conio.h>
#include <iostream>

char READ = 'r';
char MODIFY = 'm';
char ORDER_NOT_FOUND = 'n';
char ORDER_FOUND = 'f';
char EXIT = 'e';
char END_OPERATION = 'd';
char END_MODIFY = 'n';

struct Order {
	int n;
	char name[10];
	int amount;
	double price;
};

int main(int argc, char* argv[]) {

	HANDLE writePipe = (HANDLE)atoi(argv[1]);
	HANDLE readPipe = (HANDLE)atoi(argv[2]);
	bool doCycle = true;
	while (doCycle) {
		DWORD bytesWrite;
		DWORD bytesRead;
		std::cout << "1) Read\n2) Modify\n3) Exit\n";
		int answer;
		std::cin >> answer;
		system("cls");
		if (answer == 1) {
			int orderNumber;
			std::cout << "Enter order number:\n";
			std::cin >> orderNumber;

			WriteFile(writePipe, &READ, sizeof(READ), &bytesWrite, NULL);
			WriteFile(writePipe, &orderNumber, sizeof(orderNumber), &bytesWrite, NULL);

			char serverAnswer;
			ReadFile(readPipe, &serverAnswer, sizeof(serverAnswer), &bytesRead, NULL);
			if (serverAnswer == ORDER_FOUND) {
				Order order;
				ReadFile(readPipe, &order, sizeof(order), &bytesRead, NULL);
				std::cout << "\nOrder name:\n" << order.name << "\nProduct count:\n" << order.amount << "\nProduct price:\n" << order.price << std::endl;
			}
			else {
				std::cout << "Order not found.\n";
			}

			char c;
			std::cout << "Press any char to finish reading: ";
			std::cin >> c;

			WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL);
		}
		else if (answer == 2) {
			int orderNumber;
			std::cout << "Enter order number:\n";
			std::cin >> orderNumber;

			WriteFile(writePipe, &READ, sizeof(READ), &bytesWrite, NULL);

			WriteFile(writePipe, &orderNumber, sizeof(orderNumber), &bytesWrite, NULL);

			char serverAnswer;
			ReadFile(readPipe, &serverAnswer, sizeof(serverAnswer), &bytesRead, NULL);

			if (serverAnswer == ORDER_FOUND) {
				Order order;

				ReadFile(readPipe, &order, sizeof(order), &bytesRead, NULL);

				WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL);

				std::cout << "\nOrder name:\n" << order.name << "\nProduct count:\n" << order.amount << "\nProduct price:\n" << order.price << std::endl << std::endl;

				std::cout << "Enter order name:\n";
				std::cin >> order.name;

				std::cout << "Enter product count:\n";
				std::cin >> order.amount;

				std::cout << "Enter product price:\n";
				std::cin >> order.price;

				WriteFile(writePipe, &MODIFY, sizeof(MODIFY), &bytesWrite, NULL);

				WriteFile(writePipe, &order, sizeof(order), &bytesWrite, NULL);
			}
			else {
				std::cout << "Order not found.\n";
			}

			char answer;
			ReadFile(readPipe, &answer, sizeof(answer), &bytesRead, NULL);

			char c;
			std::cout << "Press any char to finish modifying: ";
			std::cin >> c;

			WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL);
		}
		else {
			WriteFile(writePipe, &EXIT, sizeof(EXIT), &bytesWrite, NULL);
			doCycle = false;
		}
	}

	CloseHandle(writePipe);
	CloseHandle(readPipe);
	return 0;
}
