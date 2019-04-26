//
// Created by zakhar on 29/01/19.
//

#pragma once

#include <armadillo>

namespace Algebra
{
namespace Algorithms
{

class RSVD
{
  public:
    explicit RSVD();
    explicit RSVD(int q);
    int get_q() const;
    void set_q(int q);
    int center(arma::mat &X) const;
    int rsvd(arma::uword k, bool retu, bool retv, const arma::mat &X);
    
    arma::vec D;
    arma::mat U;
    arma::mat V;
    
    static int rsvd(arma::mat &U, arma::vec &S, arma::mat &V, const arma::mat &X, uint64_t k);
    static void print_error(int code);
  
  private:
    int default_q();
    int q_;
};

} // namespace Algorithms
} // namespace Algebra

