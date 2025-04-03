#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#define ARRAY_SIZE 1000000

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = ARRAY_SIZE / size;
    std::vector<int> local_array(chunk_size);
    std::vector<int> array;

    if (rank == 0) {
        array.resize(ARRAY_SIZE);
        std::srand(std::time(nullptr));
        for (int i = 0; i < ARRAY_SIZE; ++i) {
            array[i] = std::rand() % 100;
        }
    }

    double start_time = MPI_Wtime();
    MPI_Scatter(array.data(), chunk_size, MPI_INT, local_array.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    int local_sum = 0;
    for (int num : local_array) {
        local_sum += num;
    }

    int global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (rank == 0) {
        std::cout << "Global Sum: " << global_sum << std::endl;
        std::cout << "Time taken: " << (end_time - start_time) << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}