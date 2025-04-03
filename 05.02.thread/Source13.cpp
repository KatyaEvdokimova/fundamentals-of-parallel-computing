#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx; // Мьютекс для защиты общей переменной

// Функция для получения минора матрицы
vector<vector<int>> getMinor(const vector<vector<int>>& matrix, int row, int col) {
    int n = matrix.size();
    vector<vector<int>> minor;
    for (int i = 0; i < n; ++i) {
        if (i == row) continue;
        vector<int> minorRow;
        for (int j = 0; j < n; ++j) {
            if (j == col) continue;
            minorRow.push_back(matrix[i][j]);
        }
        minor.push_back(minorRow);
    }
    return minor;
}

// Рекурсивная функция для вычисления определителя
int determinant(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    if (n == 1) return matrix[0][0];
    if (n == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

    int det = 0;
    vector<thread> threads;
    vector<int> results(n, 0);

    // Функция для вычисления минора в отдельном потоке
    auto computeMinor = [&](int col) {
        vector<vector<int>> minor = getMinor(matrix, 0, col);
        int sign = (col % 2 == 0) ? 1 : -1;
        int minorDet = determinant(minor);
        mtx.lock();
        results[col] = sign * matrix[0][col] * minorDet;
        mtx.unlock();
    };

    // Создание потоков
    for (int col = 0; col < n; ++col) {
        threads.emplace_back(computeMinor, col);
    }

    // Ожидание завершения всех потоков
    for (auto& th : threads) {
        th.join();
    }

    // Суммирование результатов
    for (int res : results) {
        det += res;
    }

    return det;
}

int main() {
    int n;
    cout << "size A: ";
    cin >> n;

    vector<vector<int>> matrix(n, vector<int>(n));
    cout << "elements of A: " << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> matrix[i][j];
        }
    }

    int det = determinant(matrix);
    cout << "det(A): " << det << endl;

    return 0;
}
