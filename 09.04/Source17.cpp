#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <complex>
#include <vector>
#include <iostream>

const int WIDTH = 1000;
const int HEIGHT = 1000;
const int MAX_ITER = 1000;

double x_min = -2.0, x_max = 1.0;
double y_min = -1.5, y_max = 1.5;

// Функция для вычисления фрактала Мандельброта
int mandelbrot(std::complex<double> c) {
    std::complex<double> z = 0;
    int iter = 0;

    while (abs(z) <= 2.0 && iter < MAX_ITER) {
        z = z * z + c;
        iter++;
    }

    return iter;
}

void generate_fractal(int rank, int size, cv::Mat& image) {
    int rows_per_process = HEIGHT / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? HEIGHT : start_row + rows_per_process;

    // Генерация фрактала для части изображения
    for (int y = start_row; y < end_row; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            double real = x_min + (x_max - x_min) * x / WIDTH;
            double imag = y_min + (y_max - y_min) * y / HEIGHT;
            std::complex<double> c(real, imag);

            int iter = mandelbrot(c);

            cv::Vec3b color(0, 0, 0);
            if (iter < MAX_ITER) {
                color[0] = (iter % 256);  // Синий
                color[1] = (iter * 5 % 256);  // Зеленый
                color[2] = (iter * 10 % 256);  // Красный
            }

            image.at<cv::Vec3b>(y, x) = color;
        }
    }
}

int main(int argc, char* argv[]) {
    // Инициализация MPI
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Создание изображения для фрактала
    cv::Mat image(HEIGHT, WIDTH, CV_8UC3);

    // Генерация фрактала параллельно
    generate_fractal(rank, size, image);

    // Сбор результатов на процессе 0
    if (rank != 0) {
        // Отправка части изображения на процесс с rank 0
        MPI_Send(image.data + rank * HEIGHT / size * WIDTH * 3, HEIGHT / size * WIDTH * 3, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }
    else {
        // Процесс 0 собирает результаты от остальных процессов
        for (int i = 1; i < size; ++i) {
            int start_row = i * HEIGHT / size;
            MPI_Recv(image.data + start_row * WIDTH * 3, HEIGHT / size * WIDTH * 3, MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Показываем результат
        cv::imshow("Mandelbrot Set", image);
        cv::waitKey(0);
    }

    
    MPI_Finalize();
    return 0;
}
