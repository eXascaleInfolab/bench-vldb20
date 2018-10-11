//
// Created by zakhar on 14.03.18.
//

#pragma once

#include "Matrix.h"

namespace LinearAlgebra_Algorithms {

    class SVDecomposition
    {
    public:
        static int64_t SVDecompose(LinearAlgebra_Basic::Matrix &u,
                                   LinearAlgebra_Basic::Vector &sigma,
                                   LinearAlgebra_Basic::Matrix &v);
    };

} // namespace LinearAlgebra_Algorithms
