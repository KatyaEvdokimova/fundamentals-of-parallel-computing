#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>

using namespace cv;
using namespace std;

void drawCarpet(Mat& img, int x, int y, int size, int depth) {
    if (depth == 0) {
        rectangle(img, Point(x, y), Point(x + size, y + size), Scalar(0, 0, 0), FILLED);
        return;
    }

    int newSize = size / 3;

#pragma omp parallel for collapse(2)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i == 1 && j == 1) {
                // Центральный квадрат остается пустым
                continue;
            }
            drawCarpet(img, x + i * newSize, y + j * newSize, newSize, depth - 1);
        }
    }
}

int main() {
    int size = 729; // Размер изображения (должен быть кратен 3^N)
    int depth = 5;  // Глубина рекурсии

    Mat img = Mat::ones(size, size, CV_8UC3) * 255; // Белый фон

#pragma omp parallel
    {
#pragma omp single
        drawCarpet(img, 0, 0, size, depth);
    }

    imshow("Sierpinski Carpet", img);
    imwrite("sierpinski_carpet.png", img);
    waitKey(0);

    return 0;
}
