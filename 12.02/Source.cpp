#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

std::mutex mtx;  // Мьютекс для синхронизации
std::condition_variable cv;
bool ready = false; // Флаг готовности ресурса
int shared_resource = 0; // Общий ресурс
void worker(int id) {
    auto start = std::chrono::high_resolution_clock::now();
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; }); // Ожидание сигнала

    // Доступ к общему ресурсу
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Поток " << id << " получил доступ к ресурсу. Значение: " << shared_resource
        << " Время ожидания: " << elapsed.count() << " секунд" << std::endl;
}

void initializer() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        shared_resource = 42; // Инициализация ресурса
        ready = true;
        std::cout << "Ресурс инициализирован" << std::endl;
    }
    cv.notify_all(); // Уведомление всех потоков
}

int main() {
    setlocale(LC_ALL, "RU");
    const int num_threads = 5;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1)); // Имитация задержки
    initializer();

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
