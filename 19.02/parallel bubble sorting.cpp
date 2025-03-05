#include <iostream>
#include <omp.h>
#include <vector>
#include <cstdlib>
#include <ctime>

void parallelBubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    bool sorted = false;

    while (!sorted) {
        sorted = true;

        // Четный проход
#pragma omp parallel for shared(arr, sorted)
        for (int i = 0; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

        // Синхронизация потоков перед следующим проходом
#pragma omp barrier

// Нечетный проход
#pragma omp parallel for shared(arr, sorted)
        for (int i = 1; i < n - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false;
            }
        }

#pragma omp barrier
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    std::srand(std::time(0));
    int n = 20;
    std::vector<int> arr(n);

    // Заполнение массива случайными числами
    for (int& x : arr) {
        x = std::rand() % 100;
    }

    std::cout << "Исходный массив: ";
    for (const int& x : arr) std::cout << x << " ";
    std::cout << std::endl;

    parallelBubbleSort(arr);

    std::cout << "Отсортированный массив: ";
    for (const int& x : arr) std::cout << x << " ";
    std::cout << std::endl;

    return 0;
}
