//
// Created by zakhar on 16/01/19.
//

#include "SVT.h"

namespace Algorithms
{

void SVT::doSVT(arma::mat &X)
{
    // const parameters
    constexpr uint64_t incre = 5;
    constexpr uint64_t rInc = 4;
    uint64_t r = 3;
    
    // dynamic parameters
    
    uint64_t n1 = X.n_rows;
    uint64_t n2 = X.n_cols;
    
    if (n2 > 30)
    {
        r = (n2 - 1) / 10;
        r++;
    }
    
    bool SMALLSCALE = n1 * n2 < 100 * 100;
    
    double df = (double)(r * (n1 + n2 - r));
    double m = std::min(5 * df, round(.99 * (double)(n1 * n2)));
    double p = m / (double)(n1 * n2);
    
    double tau = 2 * sqrt((double)(n1 * n2)); // modified from 5 * sqrt(...)
    double delta = 1.2 / p;
    
    // initialization of sparse matrix
    
    arma::uvec omega = arma::find_finite(X);
    uint64_t n_observed = omega.n_elem;
    
    arma::vec data(n_observed);
    arma::umat observed(2, n_observed);
    
    uint64_t counter = 0;
    for (uint64_t i = 0; i < n1; ++i)
    {
        for (uint64_t j = 0; j < n2; ++j)
        {
            if (!std::isnan(X.at(i, j)))
            {
                observed.at(0, counter) = i;
                observed.at(1, counter) = j;
                data[counter] = X.at(i, j);
                counter++;
            }
        }
    }
    
    
    arma::vec b = X.elem(omega);
    
    arma::sp_mat Y(observed, data, X.n_rows, X.n_cols);
    
    // rest of parameters & init process
    
    uint64_t rank = 0;
    double k0 = std::ceil(tau / (delta * arma::norm(Y, 2)));
    double normb = arma::norm(b);
    
    arma::vec y = k0 * delta * b;
    updateSparse(Y, y, omega);
    
    // recovery
    
    arma::mat U;
    arma::vec S;
    arma::mat V;
    
    for (uint64_t k = 0; k < max_iter; ++k)
    {
        uint64_t s = std::min(rank + rInc, std::min(n1, n2));
        
        if (SMALLSCALE)
        {
            bool code = arma::svd_econ(U, S, V, arma::mat(Y), "both", "std"); // mat() constructor to convert to dense
            if (!code)
            {
                std::cout << "economical svd failed, aborting recovery" << std::endl;
                return;
            }
        }
        else
        {
            bool OK = false;
            
            while (!OK)
            {
                bool code = arma::svds(U, S, V, Y, s);
                if (!code)
                {
                    std::cout << "sparse svd failed, aborting recovery" << std::endl;
                    return;
                }
                if (S.n_elem < s)
                {
                    OK = true;
                }
                else
                {
                    OK = (S(s - 1) <= tau) || (s == std::min(n1, n2));
                }
                s = std::min(s + incre, std::min(n1, n2));
            }
        }
        
        rank = arma::sum(S > tau);
        
        rank = rank > 0 ? rank : 1;
        
        U = U.submat(arma::span::all, arma::span(0, rank - 1));
        V = V.submat(arma::span::all, arma::span(0, rank - 1));
        S = S.subvec(arma::span(0, rank - 1));
        
        for (uint64_t i = 0; i < rank; ++i)
        {
            S[i] -= tau;
        }
        
        arma::vec x = XonOmega(U * arma::diagmat(S), V, omega);
        
        double relRes = arma::norm(x - b) / normb;
        
        if (relRes < tol)
        {
            break;
        }
        
        if (arma::norm(x - b) / normb > 1e5)
        {
            std::cout << "Divergence!" << std::endl;
            X = U * arma::diagmat(S) * V.t();
            break;
        }
        
        y = y + delta * (b - x);
        
        updateSparse(Y, y, omega);
    }
    
    // replace original matrix with the recovered
    X = U * arma::diagmat(S) * V.t();
}

void SVT::updateSparse(arma::sp_mat &Y, const arma::vec &b, const arma::uvec &omega)
{
    for (uint64_t i = 0; i < omega.n_elem; ++i)
    {
        Y(omega[i]) = b[i];
    }
}

arma::vec SVT::XonOmega(const arma::mat &U_S, const arma::mat &V, const arma::uvec &omega)
{
    uint64_t M = U_S.n_rows;
    uint64_t N = V.n_rows;
    bool FULLMATRIX = M * N < 50 * 50;
    
    if (omega.n_elem == M * N)
    {
        FULLMATRIX = true;
    }
    
    if (FULLMATRIX)
    {
        arma::mat A = U_S * V.t();
        return A.elem(omega);
    }
    else
    {
        arma::vec y(omega.n_elem);
        
        for (uint64_t k = 0; k < omega.n_elem; k++)
        {
            // [!] indexing only works for column oriented layout
            y[k] = arma::dot(U_S.row(omega[k] % M), V.row(omega[k] / M).t());
        }
        return y;
    }
}

} // namespace Algorithms
