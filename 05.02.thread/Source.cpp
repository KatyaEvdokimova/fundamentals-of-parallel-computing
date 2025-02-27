#include <iostream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

mutex mtx;

// Вычисление факториала
void partial_factorial(long long start, long long end, long long& result) {
    long long temp = 1;
    for (long long i = start; i <= end; ++i) {
        temp *= i;
    }
    lock_guard<mutex> lock(mtx);
    result *= temp;
}

// Функция для вычисления факториала в многопоточном режиме
long long factorial_multithreaded(int n, int num_threads) {
    if (n == 0 || n == 1) return 1;

    vector<thread> threads;
    vector<long long> results(num_threads, 1);
    int chunk_size = n / num_threads;
    int remainder = n % num_threads;

    int start = 1;
    for (int i = 0; i < num_threads; ++i) {
        int end = start + chunk_size - 1;
        if (i < remainder) ++end;

        threads.emplace_back(partial_factorial, start, end, ref(results[i]));
        start = end + 1;
    }

    for (auto& th : threads) {
        th.join();
    }

    long long result = 1;
    for (const auto& res : results) {
        result *= res;
    }
    return result;
}


long long factorial(int n) {
    long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    setlocale(LC_ALL, "Ru");
    int n, num_threads;
    cout << "Введите число: ";
    cin >> n;
    cout << "Введите количество потоков: ";
    cin >> num_threads;

    auto start_time = chrono::high_resolution_clock::now();
    long long result_single = factorial(n);
    auto end_time = chrono::high_resolution_clock::now();
    cout << "Факториал (один поток): " << result_single << endl;
    cout << "Время выполнения (один поток): " << chrono::duration<double>(end_time - start_time).count() << " сек." << endl;

    start_time = chrono::high_resolution_clock::now();
    long long result_multi = factorial_multithreaded(n, num_threads);
    end_time = chrono::high_resolution_clock::now();
    cout << "Факториал (многопоточно): " << result_multi << endl;
    cout << "Время выполнения (многопоточно): " << chrono::duration<double>(end_time - start_time).count() << " сек." << endl;

    return 0;
}