//
// Created on 16/01/19.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

class SVT
{
    static constexpr double tol = 1E-4;
    static constexpr uint64_t max_iter = 100;
  
  public:
    static void doSVT(arma::mat &X, double tauScale);
  
  private:
    static void updateSparse(arma::sp_mat &Y, const arma::vec &b, const arma::uvec &omega);
    
    static arma::vec XonOmega(const arma::mat &U_S, const arma::mat &V, const arma::uvec &omega);
};


} // namespace Algorithms
