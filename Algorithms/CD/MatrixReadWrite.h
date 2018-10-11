//
// Created by Zakhar on 16/03/2017.
//

#ifndef CENTROIDDECOMPOSITION_MATRIXREADWRITE_H
#define CENTROIDDECOMPOSITION_MATRIXREADWRITE_H

#include <string>
#include "LinearAlgebra/Matrix.h"
#include "LinearAlgebra/MissingBlock.h"

namespace MathIO {

double *getMatrixByName(std::string name, uint64_t n, uint64_t m);
std::vector<double> *getNextValue(uint64_t m);
void closeFile();

void writeTime(std::string out, uint64_t n, uint64_t m, int64_t time);
void writeMemory(std::string out, uint64_t n, uint64_t m, uint64_t memory);
void writePrecision(std::string out, uint64_t n, uint64_t m, double precision);
void writeRecovery(std::string out, uint64_t n, uint64_t m, int64_t result,
                   std::vector<LinearAlgebra_Algorithms::MissingBlock> *missingBlocks);
void writeMatrix(std::string out, LinearAlgebra_Basic::Matrix &mx);

template<class T>
void writeVector(const std::string &out, std::vector<T> vector);

extern template void writeVector<double>(const std::string &out, std::vector<double> vector);


} //namespace MathIO

#endif //CENTROIDDECOMPOSITION_MATRIXREADWRITE_H
