//
// Created by zakhar on 21/01/19.
//

#include "SoftImpute.h"
#include "../Algebra/RSVD.h"

#include <iostream>

namespace Algorithms
{

bool _converged(const arma::mat &X_old, const arma::mat &X_new, const std::vector<arma::uvec> &indices, double threshold)
{
    double delta = 0.0;
    double old_norm = 0.0;
    for (uint64_t i = 0; i < X_old.n_cols; ++i)
    {
        for (uint64_t j : indices[i])
        {
            old_norm += X_old.at(j, i) * X_old.at(j, i);
            double diff = X_old.at(j, i) - X_new.at(j, i);
            delta += diff * diff;
        }
    }
    
    return old_norm > arma::datum::eps && (delta / old_norm) < threshold;
}

arma::mat _svd_step(arma::mat &X, double shrinkage_value, uint64_t max_rank, uint64_t &rank)
{
    arma::mat U;
    arma::vec S;
    arma::mat V;
    
    int code = Algebra::Algorithms::RSVD::rsvd(U, S, V, X, max_rank);
    
    if (code != 0)
    {
        std::cout << "RSVD returned an error: ";
        Algebra::Algorithms::RSVD::print_error(code);
        std::cout << ", aborting remaining recovery" << std::endl;
        return arma::mat();
    }
    
    for (uint64_t i = 0; i < S.n_elem; ++i)
    {
        S[i] = std::max(S[i] - shrinkage_value, 0.0);
    }
    
    rank = std::min(max_rank, (uint64_t) arma::sum(S > 0.0));
    
    return U(arma::span::all, arma::span(0, rank - 1)) * arma::diagmat(S(arma::span(0, rank - 1))) * ((arma::mat)V.t())(arma::span(0, rank - 1), arma::span::all);
}

void SoftImpute::doSoftImpute(arma::mat &X, uint64_t max_rank)
{
    // --defaults for the algorithms from FancyImpute:
    // fill_method="zero"
    // min_value=None
    // max_value=None
    // normalizer=None
    //
    // -- defaults for SoftImpute:
    // shrinkage_value=None
    // convergence_threshold=0.001
    // max_iters=100
    // max_rank=None
    // n_power_iterations=1
    // init_fill_method="zero"
    
    // need for any fancyimpute:
    // missingmask, fill with fill_method (see. function for nonzero fills), solve(), done
    
    constexpr uint64_t max_iters = 100;
    constexpr double threshold = 0.00001;
    
    std::vector<arma::uvec> indices;
    
    for (uint64_t i = 0; i < X.n_cols; ++i)
    {
        indices.emplace_back(arma::find_nonfinite(X.col(i)));
    }
    
    for (uint64_t i = 0; i < X.n_cols; ++i)
    {
        for (uint64_t j : indices[i])
        {
            X.at(j, i) = 0.0;
        }
    }
    
    // begin solve()
    
    double shrinkage_value = arma::svd(X)[0] / 50.0;
    
    uint64_t iter = 0;
    while (iter < max_iters)
    {
        uint64_t rank = 0;
        arma::mat X_reconstructed = _svd_step(X, shrinkage_value, max_rank, rank);
        
        if (X_reconstructed.n_elem == 0)
        {
            return;
        }
    
        bool conv = _converged(X, X_reconstructed, indices, threshold);
        
        for (uint64_t i = 0; i < X.n_cols; ++i)
        {
            for (uint64_t j : indices[i])
            {
                X.at(j, i) = X_reconstructed.at(j, i);
            }
        }
        
        if (conv)
        {
            break;
        }
        
        ++iter;
    }
}

} // namespace Algorithms
