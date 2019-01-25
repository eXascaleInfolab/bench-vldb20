//
// Created by zakhar on 04/01/19.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

struct DynaMMoModel
{
    arma::mat A;
    arma::mat C;
    arma::mat Q;
    arma::mat R;
    arma::vec mu0;
    arma::mat Q0;
  
  public:
    explicit DynaMMoModel(arma::mat &&_A, arma::mat &&_C, arma::mat &&_Q,
                          arma::mat &&_R, arma::vec &&_mu0, arma::mat &&_Q0)
            : A(_A), C(_C), Q(_Q), R(_R), mu0(_mu0), Q0(_Q0)
    { }
};

class DynaMMo
{
  public:
    static void doDynaMMo(arma::mat &X, uint64_t H = 0, uint64_t maxIter = 100, bool FAST = false);
  
  private:
    static std::tuple<std::vector<arma::vec>, std::vector<arma::mat>, std::vector<arma::mat>>
    forward(const arma::mat &X, const DynaMMoModel &model, uint64_t N, uint64_t H, bool FAST);
    
    static std::tuple<std::vector<arma::vec>, std::vector<arma::mat>, std::vector<arma::mat>>
    backward(const std::vector<arma::vec> &mu, const std::vector<arma::mat> &V, const std::vector<arma::mat> &P, const DynaMMoModel &model, uint64_t N);
    
    static void
    estimate_missing(arma::mat &Y, uint64_t N, const std::vector<arma::vec> &Ez, const DynaMMoModel &model);
    
    static void
    MLE_lds(uint64_t N, uint64_t M, uint64_t H, const arma::mat &X, DynaMMoModel &model,
            std::vector<arma::vec> Ez, std::vector<arma::mat> Ezz, std::vector<arma::mat> Ez1z);
};

} // namespace Algorithms

