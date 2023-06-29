#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define M 4
#define N 5
double table[M][N] = {
    {1.0, 2.0, 3.0, 4.0, 5.0},
    {2.0, 3.0, 4.0, 5.0, 6.0},
    {3.0, 4.0, 5.0, 6.0, 7.0},
    {4.0, 5.0, 6.0, 7.0, 8.0}
};

double integrate(double* func, int n, double h) {
    double sum = 0.0;
    int i;

    sum += func[0] + func[n - 1];
    for (i = 1; i < n - 1; i++) {
        if (i % 2 == 0) {
            sum += 2 * func[i];
        }
        else {
            sum += 4 * func[i];
        }
    }

    return sum * ((h / 3.0) * ((double)(n - 1) / 2.0));
}

int main(int argc, char* argv[]) {
    int rank, numprocs;
    double local_result = 0.0;
    double global_result = 0.0;
    double local_func[N];
    double h = 1.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    int rows_per_process = M / numprocs;
    int rows_remaining = M % numprocs;

    int local_rows = (rank < rows_remaining) ? rows_per_process + 1 : rows_per_process;
    int local_size = local_rows * N;
    local_func = (double*)malloc(local_size * sizeof(double));

    MPI_Scatter(table, local_size, MPI_DOUBLE, local_func, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_rows; i++) {
        double* current_row = local_func + i * N;
        local_result += integrate(current_row, N, h);
    }

    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Global result: %f\n", global_result);
    }

    MPI_Finalize();

    return 0;
}

// cd C : \Users\solidus66\OneDrive\paod\paod6\Debug
