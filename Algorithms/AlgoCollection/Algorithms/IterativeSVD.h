//
// Created by zakhar on 21/01/19.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

class IterativeSVD
{
  public:
    static void recoveryIterativeSVD(arma::mat &X, uint64_t rank);
};

} // namespace Algorithms
