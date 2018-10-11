//
// Created by zakhar on 13.03.18.
//

#include "../LinearAlgebra/Matrix.h"

#pragma once

namespace Stats {

    class CorrelationMatrix {
        //
        // Data
        //
    private:
        LinearAlgebra_Basic::Matrix *Src;
        LinearAlgebra_Basic::Matrix *correlation;

        std::vector<double> mean;
        std::vector<double> stddev;

        //
        // Constructors & destructors
        //
    public:
        explicit CorrelationMatrix(LinearAlgebra_Basic::Matrix *mx);

        //
        // API
        //
    public:
        LinearAlgebra_Basic::Matrix *getCorrelationMatrix();
        LinearAlgebra_Basic::Vector *getSingularValuesOfCM();
        void normalizeMatrix();
        void deNormalizeMatrix();

        //
        // Algorithm
        //
    private:
        void setMeanAndStdDev();
        double getCorrelation(uint64_t col1, uint64_t col2);
    };

} // namespace Stats
