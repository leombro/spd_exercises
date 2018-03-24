//
// Created by Orlando Leombruni on 03/03/2018.
//

// A program to experiment with MPI datatypes. Process rank 0 initializes an NxN
// matrix and fills it with "1"s, then sends it to process rank 1 that multiplies
// the result by 10 and gives it back to rank 0. Depending on the datatype used
// to send/receive data, the final result change.

// This program sends the principal diagonal of the matrix.

#include <mpi.h>
#include "printMatrix.h"

using namespace std;

#define N 15

int main(int argc, char** argv) {

    int A[N][N];
    int B[N][N];

    int rank;
    int size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) return -1;

    int lengths[N];
    int displacements[N];

    for (int i{0}; i < N ; ++i) {
        lengths[i] = 1;
        displacements[i] = N*i + i;
    }

    MPI_Datatype downdiagonal;
    MPI_Type_indexed(N, lengths, displacements, MPI_INT, &downdiagonal);
    MPI_Type_commit(&downdiagonal);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {

        for (int i{0}; i < N; ++i) {
            for (int j{0}; j < N; ++j) {
                A[i][j] = 1;
            }
        }

        MPI_Send(A, 1, downdiagonal, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(A, 1, downdiagonal, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printMatrix(rank, "received", N, (int*) A);

    } else if (rank == 1) {

        MPI_Recv(B, 1, downdiagonal, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i{0}; i < N; ++i) {
            for (int j{0}; j < N; ++j) {
                B[i][j] *= 10;
            }
        }

        MPI_Send(B, 1, downdiagonal, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Type_free(&downdiagonal);
    MPI_Finalize();
}