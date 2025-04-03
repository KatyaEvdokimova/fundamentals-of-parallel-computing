#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#define N 500  // Размер матрицы

void generateMatrix(std::vector<std::vector<int>>& matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = N / size;
    std::vector<std::vector<int>> A(N, std::vector<int>(N));
    std::vector<std::vector<int>> B(N, std::vector<int>(N));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));
    std::vector<int> local_A(rows_per_proc * N);
    std::vector<int> local_C(rows_per_proc * N, 0);

    if (rank == 0) {
        srand(time(0));
        generateMatrix(A);
        generateMatrix(B);
    }

    MPI_Bcast(&B[0][0], N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&A[0][0], rows_per_proc * N, MPI_INT, &local_A[0], rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                local_C[i * N + j] += local_A[i * N + k] * B[k][j];
            }
        }
    }

    MPI_Gather(&local_C[0], rows_per_proc * N, MPI_INT, &C[0][0], rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Sample output: " << C[0][0] << " " << C[N/2][N/2] << " " << C[N-1][N-1] << std::endl;
    }

    MPI_Finalize();
    return 0;
}
