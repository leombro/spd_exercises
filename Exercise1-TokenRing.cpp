//
// Created by Orlando Leombruni on 02/03/2018.
//

// A program where each node receives a string from its predecessor, appends
// a new string (either ping or pong depending on the process' rank), and 
// sends the result to its successor. This is organized as a circular ring
// where the process with rank 0 both starts and ends the computation.

#include <mpi.h>
#include <iostream>

using namespace std;

void pingponger(int rank, int succ, char* buf, int size, MPI_Comm communicator) {
    MPI_Recv(buf, size, MPI_CHAR, (rank - 1), 0, communicator, MPI_STATUS_IGNORE);
    strcat(buf, (rank % 2 == 0) ? " ping" : " pong");
    MPI_Send(buf, size, MPI_CHAR, succ, 0, communicator);
}

int main(int argc, char** argv) {
    int myRank;
    int size;
    char buffer[500];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (myRank == 0) {
        if (size > 1) {
            char toSend[5] = "ping";
            MPI_Send(&toSend, 5, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&buffer, 500, MPI_CHAR, (size - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "Rank0: received " << buffer << " from Rank" << (size - 1) << endl;
        }
        else {
            cout << "Only one node, nothing to do." << endl;
        }
    }
    else {
        pingponger(myRank, (myRank + 1 < size) ? myRank + 1 : 0, buffer, 500, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}