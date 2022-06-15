#include <windows.h>
#include <iostream>
#include <fstream>
#include <math.h>

#pragma warning(disable : 4996)

using namespace std;

volatile double* polynoms = new double[2];

struct polynom {
	int degree;
	double* coef;
	double x;
	bool is_del;


	polynom(int degree, double* coef, double x,int is_del) {
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
	if (num.is_del == 0) cout << "Числитель P(x) = " << result << endl;
	else cout << "Знаменатель P(x) = " << result << endl;
	polynoms[num.is_del] = result;
	return 0;
}


int main()
{
	setlocale(LC_ALL, "rus");
	int num_d;
	cout << "Введите степень числителя:\n";
	cin >> num_d;

	double* num_c = new double[num_d + 1];
	cout << "Введите коэффициенты числителя:\n";
	for (int i = 0; i < num_d + 1; i++) {
		cin >> num_c[i];
	}

	int den_d;
	cout << "Введите степень знаменателя:\n";
	cin >> den_d;

	double* den_c = new double[den_d + 1];
	cout << "Введите коэффициенты знаменателя\n";
	for (int i = 0; i < den_d + 1; i++) {
		cin >> den_c[i];
	}

	double x;
	cout << "Введите значение x:\n";
	cin >> x;

	HANDLE hThread[2];
	DWORD IDThread[2];

	polynom* num = new polynom(num_d, num_c, x, 0);
	polynom* den = new polynom(den_d, den_c, x, 1);


	hThread[0] = CreateThread(NULL, 0, Polynom, (void*)num, 0, &IDThread[0]);
	if (hThread[0] == NULL) {
		cout << "Процесс polynom подсчёта числителя не запустился";
		return GetLastError();
	}

	hThread[1] = CreateThread(NULL, 0, Polynom, (void*)den, 0, &IDThread[1]);
	if (hThread[1] == NULL) {
		cout << "Процесс polynom подсчёта знаменателя не запустился";
		return GetLastError();
	}


	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);


	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);


	if (polynoms[1] == 0) {
		cout << "Знаменатель равен 0, деление невозможно" << endl;
	}
	else {
		cout << "f(x) = " << polynoms[0] / polynoms[1] << endl;
	}

	return 0;
}