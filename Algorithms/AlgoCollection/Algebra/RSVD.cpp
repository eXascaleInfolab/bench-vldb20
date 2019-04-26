//
// Created by zakhar on 29/01/19.
//

//
// Code taken from: https://github.com/RBigData/rsvd
//

#include "RSVD.h"

namespace Algebra
{
namespace Algorithms
{

// center.cpp

int RSVD::center(arma::mat &X) const
{
    const arma::uword m = X.n_rows;
    const arma::uword n = X.n_cols;
    
    for (arma::uword j = 0; j < n; j++)
    {
        double mean = 0.0;
        for (arma::uword i = 0; i < m; i++)
        {
            mean += X(i, j);
        }
        
        mean /= (double)m;
        
        for (arma::uword i = 0; i < m; i++)
        {
            X(i, j) -= mean;
        }
    }
    
    return 0;
}

// scaffolding.cpp

int RSVD::default_q()
{
    return 3;
}

RSVD::RSVD()
{
    q_ = default_q();
}

RSVD::RSVD(int q)
{
    q_ = q;
}

int RSVD::get_q() const
{
    return q_;
}

void RSVD::set_q(const int q)
{
    q_ = q;
}

// rsvd.cpp

#define THROW_QR \
  return 1;


int RSVD::rsvd(const arma::uword k, const bool retu, const bool retv, const arma::mat &X)
{
    bool check;
    const arma::uword n = X.n_cols;
    arma::mat Omega;
    arma::mat Q, R;
    arma::vec B_D;
    arma::mat B, B_U, B_V;
    
    if (k < 1)
    {
        throw std::invalid_argument("'k' must be positive");
    }
    else if (k > n)
    {
        throw std::invalid_argument("'k' must be less than nrows(X)");
    }
    
    if (X.n_rows < 1 || n < 1)
    {
        throw std::invalid_argument("# of rows/cols of 'X' must each be >= 1");
    }
    
    // Stage A from the paper
    try
    {
        Omega.set_size(n, 2 * k);
    }
    catch (...)
    {
        return 10;
    }
    
    arma::arma_rng::set_seed(18931);
    Omega.randu();
    
    arma::mat Y = X * Omega;
    
    check = qr_econ(Q, R, Y);
    
    for (int i = 0; i < q_; i++)
    {
        Y = X.t() * Q;
        check = qr_econ(Q, R, Y);
        if (!check)
            THROW_QR;
        
        Y = X * Q;
        check = qr_econ(Q, R, Y);
        if (!check)
            THROW_QR;
    }
    
    
    // Stage B
    B = Q.t() * X;
    check = svd_econ(B_U, B_D, B_V, B);
    if (!check)
    {
        return 2;
    }
    
    
    // Wrangle return
    D = B_D.subvec(0, k - 1);
    
    if (retu)
    {
        U = Q * B_U;
        U = U(arma::span::all, arma::span(0, k - 1));
    }
    else
    {
        U = arma::mat(0, 0);
    }
    
    if (retv)
    {
        //V = B_V(arma::span(0, k - 1), arma::span::all); // this is wrong, it assumes V is transposed, which it is not; U*S*V.t() gives dimensional mismatch
        V = B_V(arma::span::all, arma::span(0, k - 1));
    }
    else
    {
        V = arma::mat(0, 0);
    }
    
    return 0;
}

// API

int RSVD::rsvd(arma::mat &U, arma::vec &S, arma::mat &V, const arma::mat &X, uint64_t k)
{
    RSVD inst = RSVD();
    
    int code = inst.rsvd(k, true, true, X);
    
    U = std::move(inst.U);
    S = std::move(inst.D);
    V = std::move(inst.V);
    
    return code;
}

void RSVD::print_error(int code)
{
    switch (code)
    {
        case 1:
            std::cout << "QR decomposition failed";
            break;
        
        case 2:
            std::cout << "SVD failed";
            break;
        
        case 10:
            std::cout << "memory allocation failed";
            break;
        
        default:
            std::cout << "unknown error occurred";
            break;
    }
}

} // namespace Algorithms
} // namespace Algebra
