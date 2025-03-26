#include <iostream>
#include <vector>
#include <cstdlib>  // Для system()
#include <thread>   // Для std::this_thread::sleep_for()
#include <chrono>   // Для std::chrono::milliseconds
#include <omp.h>

using namespace std;

const int WIDTH = 30;   // Ширина поля
const int HEIGHT = 20;  // Высота поля
const int STEPS = 100;  // Количество шагов симуляции

// Функция очистки экрана
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Функция для инициализации случайного состояния поля
void initialize(vector<vector<int>>& grid) {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            grid[i][j] = rand() % 2; // Случайное заполнение 0 или 1
        }
    }
}

// Функция отображения поля в консоли
void display(const vector<vector<int>>& grid) {
    clearScreen();
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            cout << (grid[i][j] ? "O" : ".") << " ";
        }
        cout << endl;
    }
}

// Функция для подсчета живых соседей клетки
int countNeighbors(const vector<vector<int>>& grid, int x, int y) {
    int count = 0;
    int dx[] = { -1, -1, -1, 0, 1, 1, 1, 0 };
    int dy[] = { -1, 0, 1, 1, 1, 0, -1, -1 };

    for (int i = 0; i < 8; ++i) {
        int nx = x + dx[i], ny = y + dy[i];
        if (nx >= 0 && nx < HEIGHT && ny >= 0 && ny < WIDTH) {
            count += grid[nx][ny];
        }
    }
    return count;
}

// Функция обновления состояния поля
void update(vector<vector<int>>& grid) {
    vector<vector<int>> newGrid = grid;

#pragma omp parallel for collapse(2)
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            int neighbors = countNeighbors(grid, i, j);
            if (grid[i][j] == 1) {
                newGrid[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            }
            else {
                newGrid[i][j] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
    grid = newGrid;
}

int main() {
    vector<vector<int>> grid(HEIGHT, vector<int>(WIDTH));
    initialize(grid);

    for (int step = 0; step < STEPS; ++step) {
        display(grid);
        update(grid);
        this_thread::sleep_for(chrono::milliseconds(500)); // Задержка в 500 мс
    }
    return 0;
}
