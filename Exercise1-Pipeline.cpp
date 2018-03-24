//
// Created by Orlando Leombruni on 02/03/2018.
//

// A program where each node receives a string from its predecessor, appends
// a new string (either ping or pong depending on the process' rank), and 
// sends the result to its successor. This is organized as a pipeline,
// where the process with rank 0 starts the computation and the process with
// rank (size - 1) ends it.

#include <mpi.h>
#include <iostream>

using namespace std;

void pipeline_node(int rank, int succ, char* buf, int size, MPI_Comm communicator) {
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

    if (size == 1) {
        cout << "Only one node, nothing to do." << endl;
    } else {
        if (myRank == 0) {
            char toSend[5] = "ping";
            MPI_Send(&toSend, 5, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        } else if (myRank == (size - 1)) {
            MPI_Recv(&buffer, 500, MPI_CHAR, (myRank - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "Rank" << myRank << ": received " << buffer << " from Rank" << (myRank - 1) << endl;
        } else {
            pipeline_node(myRank, myRank + 1, buffer, 500, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();

    return 0;
}