//
// Created by Orlando Leombruni on 24/03/2018.
//

#ifndef MPI_PRINTMATRIX_H
#define MPI_PRINTMATRIX_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

void printMatrix(int rank, const std::string &message, int n, int (*m)) {
    std::stringstream s{};
    s << "Rank" << rank << ": " << message << std::endl;
    for (int i{0}; i < n; ++i) {
        for (int j{0}; j < n; ++j) {
            s << std::setw(5) << m[i*n+j];
        }
        s << '\n';
    }
    std::cout << s.str() << std::endl;
}

#endif //MPI_PRINTMATRIX_H
