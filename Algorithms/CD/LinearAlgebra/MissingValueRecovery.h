//
// Created by zakhar on 05.03.18.
//

#ifndef INCCD_MISSINGVALUERECOVERY_H
#define INCCD_MISSINGVALUERECOVERY_H

#include <cmath>
#include "CentroidDecomposition.h"
#include "Matrix.h"
#include "MissingBlock.h"

namespace LinearAlgebra_Algorithms {

    class MissingValueRecovery {
        //
        // Data
        //
    private:
        LinearAlgebra_Basic::Matrix *Src;
        CentroidDecomposition *cd;
        uint64_t k;

    public:
        const uint64_t maxIterations;
        double epsPrecision;
        std::vector<MissingBlock> missingBlocks;

        //
        // Constructors & desctructors
        //
    public:
        explicit MissingValueRecovery(LinearAlgebra_Basic::Matrix *Src, uint64_t maxIterations = 50, double eps = 1E-11);

        //
        // API
        //
    public:
        uint64_t getReduction();
        void setReduction(uint64_t k);

        void addMissingBlock(uint64_t col, uint64_t start, uint64_t size);
        void addMissingBlock(MissingBlock mb);
        void autoDetectMissingBlocks(double val = NAN);
        void decomposeOnly();
        void increment(std::vector<double> &newData);
        uint64_t performRecovery(bool determineReduction = false, bool useCache = false, uint32_t optimization = 0);

        //
        // Algorithm
        //
    private:
        void initialize();
        void determineReduction();
    };
} // namespace LinearAlgebra_Algorithms


#endif //INCCD_MISSINGVALUERECOVERY_H
