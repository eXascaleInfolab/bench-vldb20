//
// Created by zakhar on 21/01/19.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

class SoftImpute
{
  public:
    static void doSoftImpute(arma::mat &X, uint64_t max_rank);
};

} // namespace Algorithms
