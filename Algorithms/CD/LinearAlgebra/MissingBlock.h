//
// Created by zakhar on 05.03.18.
//

#ifndef INCCD_MISSINGBLOCK_H
#define INCCD_MISSINGBLOCK_H

#include <cstdlib>
#include <cstdint>
#include "Matrix.h"

namespace LinearAlgebra_Algorithms {

    class MissingBlock {
        //
        // Data
        //
    public:
        const uint64_t column;
        const uint64_t startingIndex;
        const uint64_t blockSize;

        const LinearAlgebra_Basic::Matrix *matrix;

        //
        // Constructors & destructors
        //
    public:
        MissingBlock(uint64_t column, uint64_t startingIndex, uint64_t blockSize, LinearAlgebra_Basic::Matrix *matrix);

    public:
        double *extractBlock();
        void imputeBlock(const double *data);
        void imputeBlock(const LinearAlgebra_Basic::Vector &data);
    };

} // namespace LinearAlgebra_Algorithms

#endif //INCCD_MISSINGBLOCK_H
