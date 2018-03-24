//
// Created by Orlando Leombruni on 03/03/2018.
//

// A program to experiment with MPI datatypes. Process rank 0 initializes an NxN
// matrix and fills it with progressive integers, then sends the lower triangular
// portion "by columns". Rank 1 receives the data as the upper triangular portion
// "by rows" and re-sends it "as-is" to rank 0. Rank 0 finally receives this last
// data as the upper triangular portion of the matrix, so the final result is a
// symmetric matrix.  

#include <mpi.h>
#include "printMatrix.h"
using namespace std;

void getMatrixDatatype(int n, MPI_Datatype* container) {
    MPI_Type_contiguous(n*n, MPI_INT, container);
    MPI_Type_commit(container);
}

void getLowerTriangularDatatype(int n, MPI_Datatype* container) {
    int lengths[n];
    int strides[n];
    for (int i{0}; i < n; i++) {
        lengths[i] = (i+1);
        strides[i] = n*i;
    }
    MPI_Type_indexed(n, lengths, strides, MPI_INT, container);
    MPI_Type_commit(container);
}

void getUpperTriangularDatatype(int n, MPI_Datatype* container) {
    int lengths_u[n];
    int strides_u[n];
    for (int i{0}; i < n; i++) {
        lengths_u[i] = (n-i);
        strides_u[i] = (n*i)+i;
    }
    MPI_Type_indexed(n, lengths_u, strides_u, MPI_INT, container);
    MPI_Type_commit(container);
}

void getLowerInvertedTriangularDatatype(int n, MPI_Datatype *container) {
    int l{((n*n)+n)/2};
    int lengths_u2[l];
    int strides_u2[l];
    for (int i{0}, els{0}; i < n; i++) {
        for (int j{0}; j < (n - i); j++) {
            lengths_u2[els] = 1;
            strides_u2[els] = (n)*(j) + (i*n) + i;
            els++;
        }
    }
    MPI_Type_indexed(l, lengths_u2, strides_u2, MPI_INT, container);
    MPI_Type_commit(container);
}

int main(int argc, char** argv) {

    int N{5};

    int A[N][N];
    int B[N][N];

    int rank;
    int size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) return -1;

    MPI_Datatype upper, lower2;
    getLowerInvertedTriangularDatatype(N, &lower2);
    getUpperTriangularDatatype(N, &upper);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        int count{1};
        for (int i{0}; i<N; i++) {
            for (int j{0}; j<N; j++) {
                A[i][j] = count++;
            }
        }

        printMatrix(rank, "initial matrix", N, (int *) A);


        MPI_Send(A, 1, lower2, 1, 0, MPI_COMM_WORLD);


        MPI_Recv(A, 1, upper, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printMatrix(rank, "received back", N, (int *) A);


    } else if (rank == 1) {

        for (int i{0}; i<N; i++) {
            for (int j{0}; j<N; j++) {
                B[i][j] = 0;
            }
        }

        MPI_Recv(B, 1, upper, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printMatrix(rank, "received", N, (int *) B);

        MPI_Send(B, 1, upper, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Type_free(&upper);
    MPI_Type_free(&lower2);
    MPI_Finalize();
}
