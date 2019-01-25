//
// Created by zakhar on 14.01.19.
//

#pragma once

#include <mlpack/core.hpp>

namespace Algorithms
{

class NMFMissingValueRecovery
{
  public:
    static void doNMFRecovery(arma::mat &input, uint64_t truncation);
};
} // namespace Algorithms
