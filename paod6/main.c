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
    int local_start_row = rank * rows_per_process + ((rank < rows_remaining) ? rank : rows_remaining);
    int local_end_row = local_start_row + local_rows;

    for (int i = local_start_row; i < local_end_row; i++) {
        for (int j = 0; j < N; j++) {
            local_func[j] = table[i][j];
        }

        local_result += integrate(local_func, N, h);
    }

    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Global result: %f\n", global_result);
    }

    MPI_Finalize();

    return 0;
}

// cd C : \Users\solidus66\OneDrive\paod\paod6\Debug