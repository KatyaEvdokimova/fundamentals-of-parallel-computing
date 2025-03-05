#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

std::mutex mtx;  // ������� ��� �������������
std::condition_variable cv;
bool ready = false; // ���� ���������� �������
int shared_resource = 0; // ����� ������

void worker(int id) {
    auto start = std::chrono::high_resolution_clock::now();
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; }); // �������� �������

    // ������ � ������ �������
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "����� " << id << " ������� ������ � �������. ��������: " << shared_resource
        << " ����� ��������: " << elapsed.count() << " ������" << std::endl;
}

void initializer() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        shared_resource = 42; // ������������� �������
        ready = true;
        std::cout << "������ ���������������" << std::endl;
    }
    cv.notify_all(); // ����������� ���� �������
}

int main() {
    setlocale(LC_ALL, "RU");
    const int num_threads = 5;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1)); // �������� ��������
    initializer();

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}