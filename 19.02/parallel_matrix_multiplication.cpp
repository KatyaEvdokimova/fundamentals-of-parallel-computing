#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;

using Matrix = vector<vector<int>>;

// Функция для генерации случайной матрицы
Matrix generateMatrix(int rows, int cols) {
    Matrix mat(rows, vector<int>(cols));
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            mat[i][j] = rand() % 10; // Заполняем случайными числами от 0 до 9
    return mat;
}

// Однопоточное умножение матриц
Matrix multiplySingleThreaded(const Matrix& A, const Matrix& B) {
    int n = A.size(), m = A[0].size(), p = B[0].size();
    Matrix C(n, vector<int>(p, 0));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < p; ++j)
            for (int k = 0; k < m; ++k)
                C[i][j] += A[i][k] * B[k][j];

    return C;
}

// Параллельное умножение матриц
Matrix multiplyParallel(const Matrix& A, const Matrix& B) {
    int n = A.size(), m = A[0].size(), p = B[0].size();
    Matrix C(n, vector<int>(p, 0));

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            int sum = 0;
            for (int k = 0; k < m; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    return C;
}

int main() {
    setlocale(LC_ALL, "RU");
    int N = 500; // Размерность матрицы

    Matrix A = generateMatrix(N, N);
    Matrix B = generateMatrix(N, N);

    // Однопоточное выполнение
    auto start = chrono::high_resolution_clock::now();
    Matrix C1 = multiplySingleThreaded(A, B);
    auto end = chrono::high_resolution_clock::now();
    cout << "Однопоточное время: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " ms" << endl;

    // Многопоточное выполнение
    start = chrono::high_resolution_clock::now();
    Matrix C2 = multiplyParallel(A, B);
    end = chrono::high_resolution_clock::now();
    cout << "Многопоточное время: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " ms" << endl;

    return 0;
}
