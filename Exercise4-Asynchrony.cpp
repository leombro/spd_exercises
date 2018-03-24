//
// Created by Orlando Leombruni on 24/03/2018.
//

// This program implements a K-asynchronous communication between two processes.
// The async_send function keeps K buffers in memory, freeing them as soon as
// they're not needed anymore. (Here, K is defined as ASYNC_GRADE = 3.)
//
// The main function for process rank 0 tries to send six times the same buffer,
// which is an integer that gets increased as soon as one "send" completes.
// Process rank 1 waits for a second and then performs six "receive"s; so, rank 0
// can send three times before blocking (on the fourth "send"), then as soon as
// rank 1 performs the first four "receive"s, it can send another two times.
//
// For convenience, the async_send function outputs to stdout whether the current
// operation is being performed asynchronously or synchronously.

#include <mpi.h>
#include <vector>
#include <iostream>
#include <thread>

#define ASYNC_GRADE 3
#define BUFSZ (5*1024)

void async_send(void* buf, int sz, MPI_Datatype type, int to, int tag, MPI_Comm communicator) {
    static char* buffers[ASYNC_GRADE];
    static int curr_pos{0};
    static int curr_async{0};
    static std::vector<std::pair<int, MPI_Request>> requests{};

    for (auto &p: requests) {
        if (p.first != -1) {
            auto req = p.second;
            int flag{0};
            MPI_Test(&req, &flag, MPI_STATUS_IGNORE);
            if (flag != 0) {
                delete[] (buffers[p.first]);
                curr_async--;
                p.first = -1;
            }
        }
    }

    if (curr_async == ASYNC_GRADE) {
        std::cout << "standard send" << std::endl;
        MPI_Ssend(buf, sz, type, to, tag, communicator);
    } else {
        std::cout << "async send" << std::endl;
        int size{0};
        MPI_Pack_size(sz, type, communicator, &size);
        auto buffer = new char[size];
        buffers[curr_pos] = buffer;
        int position{0};
        MPI_Pack(buf, sz, type, buffers[curr_pos], BUFSZ, &position, communicator);
        MPI_Request r;
        MPI_Issend(buffers[curr_pos], size, MPI_PACKED, to, tag, communicator, &r);
        requests.emplace_back(std::pair<int, MPI_Request>(curr_pos, r));
        curr_pos++;
        if (curr_pos == ASYNC_GRADE) curr_pos = 0;
        curr_async++;
    }
}

void recv(void* buf, int sz, MPI_Datatype type, int from, int tag, MPI_Comm communicator, MPI_Status *status) {
    char buffer[BUFSZ];
    int position{0};
    MPI_Recv(&buffer, BUFSZ, MPI_PACKED, from, tag, communicator, status);
    MPI_Unpack(&buffer, BUFSZ, &position, buf, sz, type, communicator);
}

int main(int argc, char** argv) {
    int myRank{0};
    int buffer;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    buffer = (myRank == 0) ? 1 : 0;

    if (myRank == 0) {
        async_send(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        std::cout << "Rank0: sent " << buffer << std::endl;
        buffer++;
        async_send(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        std::cout << "Rank0: sent " << buffer << std::endl;
        buffer++;
        async_send(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        std::cout << "Rank0: sent " << buffer << std::endl;
        buffer++;

        async_send(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // this should block
        std::cout << "Rank0: sent " << buffer << std::endl;
        buffer++;

        async_send(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        std::cout << "Rank0: sent " << buffer << std::endl;
        buffer++;
        async_send(&buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        std::cout << "Rank0: sent " << buffer << std::endl;
        buffer++;
    }

    else if (myRank == 1) {

        std::cout << "Rank1: waiting" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        recv(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Rank1: received " << buffer << std::endl;
        recv(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Rank1: received " << buffer << std::endl;
        recv(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Rank1: received " << buffer << std::endl;
        recv(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Rank1: received " << buffer << std::endl;

        std::cout << "Rank1: waiting" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        recv(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Rank1: received " << buffer << std::endl;
        recv(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Rank1: received " << buffer << std::endl;
    }

    MPI_Finalize();
}
