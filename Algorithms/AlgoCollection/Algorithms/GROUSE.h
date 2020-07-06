//
// Created on 04/01/19.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

class GROUSE
{
  public:
    static void doGROUSE(arma::mat &input, uint64_t maxrank);
  
  private:
    static constexpr uint64_t maxCycles = 5;
    static constexpr double step_size = 0.1;
};

} // namespace Algorithms

