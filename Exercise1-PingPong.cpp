//
// Created by Orlando Leombruni on 02/03/2018.
//

// A simple ping-pong program to familiarize with MPI concepts.

#include <mpi.h>
#include <iostream>
#include <sstream>
using namespace std;

int main(int argc, char** argv) {
    int myRank;
    int numberIter;
    int buffer;
    int count{0};

    if (argc == 1) {
        cout << "Please insert the number of iterations." << endl;
        return -1;
    }

    istringstream iss{argv[1]};
    if (!(iss >> numberIter)) {
        cout << "Please insert the number of iterations." << endl;
        return -1;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    buffer = (myRank == 0) ? 1 : 0;

    while (count != numberIter) {
        if (myRank == 0) {
            MPI_Send(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            cout << "Rank0: sent " << buffer << endl;
            MPI_Recv(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "Rank0: received " << buffer << endl;
            count = buffer;
        }
        else if (myRank == 1) {
            MPI_Recv(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "Rank1: received " << buffer << endl;
            buffer++;
            MPI_Send(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            cout << "Rank1: sent " << buffer << endl;
            count = buffer;
        }
    }

    MPI_Finalize();

    return 0;
}