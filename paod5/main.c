#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "mpi.h"

int main() {
    srand(time(NULL));
    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);


    int cols = 2;
    int* rows = &size; int* colsP = &cols;

    int(*matrix_1)[2] = malloc(sizeof(int) * (*rows) * cols);
    int(*matrix_2)[2] = malloc(sizeof(int) * (*rows) * cols);

    int(*finalMatrix)[2 * 2] = NULL;

    const int newMatrixCols = 4;
    int sendRow[4];

    if (rank == 0) {
        finalMatrix = malloc(sizeof(int) * (*rows) * cols * 2);

        puts("\nFirst matrix is:");
        for (int i = 0; i < (*rows); i++) {
            for (int j = 0; j < cols; j++) {
                if (j == 0) {
                    matrix_1[i][j] = i + 1;
                }
                else {
                    matrix_1[i][j] = rand() % 100;
                }
                printf("%d ", matrix_1[i][j]);
            }
            puts("");
        }

        puts("\nSecond matrix is:");
        for (int i = 0; i < (*rows); i++) {
            for (int j = 0; j < cols; j++) {
                matrix_2[i][0] = (*rows) - i;
                matrix_2[i][1] = rand() % 100;
                printf("%d ", matrix_2[i][j]);
            }
            puts("");
        }
    }

    MPI_Bcast(matrix_2, cols * (*rows), MPI_INT, 0, comm);
    MPI_Scatter(matrix_1, 2, MPI_INT, sendRow, 2, MPI_INT, 0, comm);

    free(matrix_1);

    for (int i = 0; i < *(rows); i++) {
        if (sendRow[0] == matrix_2[i][0]) {
            sendRow[2] = matrix_2[i][0];
            sendRow[3] = matrix_2[i][1];
        }
    }

    free(matrix_2);


    MPI_Gather(sendRow, newMatrixCols, MPI_INT, finalMatrix, newMatrixCols, MPI_INT, 0, comm);

    if (rank == 0) {
        puts("\nFinal matrix is:");
        for (int i = 0; i < (*rows); i++) {

            for (int j = 0; j < cols * 2; j++) {
                fprintf(stdout, "%d ", finalMatrix[i][j]);
            }
            puts("");
        }
    }

    MPI_Finalize();
    return 0;
}