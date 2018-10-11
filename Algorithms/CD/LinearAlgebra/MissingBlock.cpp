//
// Created by zakhar on 05.03.18.
//

#include "MissingBlock.h"

using namespace LinearAlgebra_Basic;
namespace LinearAlgebra_Algorithms {

    MissingBlock::MissingBlock(uint64_t column,
                               uint64_t startingIndex,
                               uint64_t blockSize,
                               LinearAlgebra_Basic::Matrix *matrix)
            : column(column),
              startingIndex(startingIndex),
              blockSize(blockSize),
              matrix(matrix)
    {
        //nothing else to do
    }

    double *MissingBlock::extractBlock() {
        double *extraction = static_cast<double *>(malloc(blockSize * sizeof(double)));
        const Matrix &mx = *matrix;

        for (uint64_t i = 0; i < blockSize; ++i)
        {
            extraction[i] = mx(startingIndex + i, column);
        }

        return extraction;
    }

    void MissingBlock::imputeBlock(const double *data) {
        const Matrix &mx = *matrix;

        for (uint64_t i = 0; i < blockSize; ++i)
        {
            mx(column, startingIndex + i) = data[i];
        }
    }

    void MissingBlock::imputeBlock(const Vector &data) {
        const Matrix &mx = *matrix;

        for (uint64_t i = 0; i < blockSize; ++i)
        {
            mx(column, startingIndex + i) = data[i];
        }
    }

} // namespace LinearAlgebra_Algorithms