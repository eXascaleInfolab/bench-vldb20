//
// Created by zakhar on 03/01/19.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

#define mod(x, y) ((((x) % (y)) + (y)) % (y))

class TKCM
{
  private:
    uint64_t l = 30;
    const uint64_t k = 3;
    const uint64_t d = 3;
    
    arma::mat &matrix;
    
    void actionTkcm(const arma::mat &ref_ts, arma::vec &ts, uint64_t &offset, const uint64_t &L);
  
  public:
    explicit TKCM(arma::mat &mx);
    
    void performRecovery();
};

} // namespace Algorithms


