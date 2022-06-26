#include <windows.h>
#include <iostream>
#include <fstream>
#include <math.h>

#pragma warning(disable : 4996)

volatile double* polynoms = new double[2];

struct polynom {
	int degree;
	double* coef;
	double x;
	bool is_del;


	polynom(int degree, double* coef, double x, int is_del) {
		this->degree = degree;
		this->coef = coef;
		this->x = x;
		this->is_del = is_del;
	}

	polynom(polynom* temp) {
		this->degree = temp->degree;
		this->coef = temp->coef;
		this->x = temp->x;
		this->is_del = temp->is_del;
	}
};


DWORD WINAPI Polynom(LPVOID temp)
{
	polynom num = new polynom((polynom*)temp);
	double result = 0.0;
	for (int i = 0; i < num.degree + 1; i++) {
		result += (num.coef[i] * pow(num.x, num.degree - i));
		Sleep(15);
	}
	Sleep(num.is_del * 15);
	if (num.is_del == 0) std::cout << "Numerator P(x) = " << result << std::endl;
	else std::cout << "Denominator P(x) = " << result << std::endl;
	polynoms[num.is_del] = result;
	return 0;
}


int main()
{
	setlocale(LC_ALL, "rus");
	int num_d;
	std::cout << "Enter numerator power:\n";
	std::cin >> num_d;

	double* num_c = new double[num_d + 1];
	std::cout << "Enter numerator coefficients:\n";
	for (int i = 0; i < num_d + 1; i++) {
		std::cin >> num_c[i];
	}

	int den_d;
	std::cout << "Enter denominator power:\n";
	std::cin >> den_d;

	double* den_c = new double[den_d + 1];
	std::cout << "Enter denominator coefficients\n";
	for (int i = 0; i < den_d + 1; i++) {
		std::cin >> den_c[i];
	}

	double x;
	std::cout << "Enter value x:\n";
	std::cin >> x;

	HANDLE hThread[2];
	DWORD IDThread[2];

	polynom* num = new polynom(num_d, num_c, x, 0);
	polynom* den = new polynom(den_d, den_c, x, 1);


	hThread[0] = CreateThread(NULL, 0, Polynom, (void*)num, 0, &IDThread[0]);
	if (hThread[0] == NULL) {
		std::cout << "Process for numerator calculating did not start";
		return GetLastError();
	}

	hThread[1] = CreateThread(NULL, 0, Polynom, (void*)den, 0, &IDThread[1]);
	if (hThread[1] == NULL) {
		std::cout << "Process for denominator calculating did not start";
		return GetLastError();
	}


	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);


	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);


	if (polynoms[1] == 0) {
		std::cout << "Denominator is 0" << std::endl;
	}
	else {
		std::cout << "f(x) = " << polynoms[0] / polynoms[1] << std::endl;
	}

	return 0;
}
