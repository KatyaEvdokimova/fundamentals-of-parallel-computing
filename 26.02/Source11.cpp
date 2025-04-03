#include <iostream>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <windows.h>

using namespace std;

int N = 1000;


double sequential() {
	double a = 0.0, b = 3.1415 / 2;
	double h = (b - a) / N;
	double sum = 0.0;

	for (int i = 0; i < N; i++) {
		double x = a + (i + 0.5) * h;
		sum += tan(x);
	}
	return sum * h;
}

// ������������ ����������
double parallel() {
	double a = 0.0, b = 3.1415 / 2;
	double h = (b - a) / N;
	double sum = 0.0;

#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < N; i++) {
		double x = a + (i + 0.5) * h;
		sum += tan(x);
	}
	return sum * h;
}

// ��������� ������� ���������� �������
double measureExecutionTime(double (*func)()) {
	auto start = chrono::high_resolution_clock::now();
	func();
	auto end = chrono::high_resolution_clock::now();
	return chrono::duration<double>(end - start).count();
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);


	cout << "������������ ����������: " << measureExecutionTime(parallel) << " ������" << endl;


	cout << "���������������� ����������: " << measureExecutionTime(sequential) << " ������" << endl;

	return 0;
}