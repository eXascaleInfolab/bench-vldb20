//
// Created by zakhar on 21/01/19.
//

#include "IterativeSVD.h"
#include "../Algebra/RSVD.h"

#include <iostream>

namespace Algorithms
{

bool converged(const arma::mat &X_old, const arma::mat &X_new, const std::vector<arma::uvec> &indices, double threshold)
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

void IterativeSVD::recoveryIterativeSVD(arma::mat &X, uint64_t rank)
{
    // --defaults for the algorithms from FancyImpute:
    // fill_method="zero"
    // min_value=None
    // max_value=None
    // normalizer=None
    //
    // -- defaults for IterSVD:
    // svd_algorithm="arpack"
    // convergence_threshold=0.00001
    // max_iters=200
    // gradual_rank_increase=True
    
    bool gradual_rank_increase = true;
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
    
    // solve()
    
    arma::mat U;
    arma::vec S;
    arma::mat V;
    
    uint64_t iter = 0;
    while (iter < max_iters)
    {
        uint64_t curr_rank;
        if (gradual_rank_increase)
        {
            curr_rank = std::min((uint64_t )std::pow(2, iter), rank);
            
            if (iter >= 20) //overflows into 0 due to 2^iter > 2^64
            {
                gradual_rank_increase = false;
            }
        }
        else
        {
            curr_rank = rank;
        }
        
        int code = Algebra::Algorithms::RSVD::rsvd(U, S, V, X, curr_rank);
        
        if (code != 0)
        {
            std::cout << "RSVD returned an error: ";
            Algebra::Algorithms::RSVD::print_error(code);
            std::cout << ", aborting remaining recovery" << std::endl;
            return;
        }

        arma::mat X_reconstructed = U(arma::span::all, arma::span(0, curr_rank - 1)) * arma::diagmat(S(arma::span(0, curr_rank - 1))) * ((arma::mat)V.t())(arma::span(0, curr_rank - 1), arma::span::all);
    
        bool conv = converged(X, X_reconstructed, indices, threshold);
    
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
