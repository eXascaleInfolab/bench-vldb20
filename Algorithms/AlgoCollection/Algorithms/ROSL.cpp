//
// Created by zakhar on 20/01/19.
//

//
// Code for ROSL taken directly from implementation: https://github.com/tjof2/robustpca
//

#include <iostream>
#include <iomanip>

#include "ROSL.h"
#include "../Algebra/Auxiliary.h"

namespace Algorithms
{

void ROSL::ROSL_Recovery(arma::mat &input, uint64_t rank, double reg)
{
    uint64_t m = input.n_rows;
    uint64_t n = input.n_cols;
    
    std::vector<arma::uvec> indices;
    
    for (uint64_t i = 0; i < input.n_cols; ++i)
    {
        indices.emplace_back(arma::find_nonfinite(input.col(i)));
    }
    
    ROSL rosl = ROSL();
    
    rosl.Parameters(rank, //initial guess
                    reg, // tba
                    1E-6, // tolerance, default value
                    500, // iterations, default value
                    0, // mode (0 = full, 1 = subsampling [not needed])
                    (uint64_t) -1, // subsampleL [not needed]
                    (uint64_t) -1, // subsampleH [not needed]
                    false // verbose
    );
    
    Algebra::Algorithms::interpolate(input, false, &indices);
    
    arma::mat reconstruction;
    
    double err = 99.0;
    uint64_t iter = 0;
    
    while (err >= 1E-7 && ++iter < 100)
    {
        rosl.runROSL(&input);
    
        rosl.D.resize(m, n);
        rosl.alpha.resize(m, n);
        
        reconstruction = rosl.D.submat(arma::span::all, arma::span(0, rosl.rank - 1)) * rosl.alpha.submat(arma::span(0, rosl.rank - 1), arma::span::all);
    
        rosl.D.reset();
        rosl.alpha.reset();
        rosl.E.reset();
        
        err = 0.0;
    
        for (uint64_t j = 0; j < input.n_cols; ++j)
        {
            for (uint64_t i : indices[j])
            {
                double lastVal = input.at(i, j);
                double newVal = reconstruction.at(i, j);
            
                err += (lastVal - newVal) * (lastVal - newVal);
            
                input.at(i, j) = newVal;
            }
        }
    
        err = std::sqrt(err);
    }
}

void ROSL::runROSL(arma::mat *X)
{
    uint64_t m = (*X).n_rows;
    uint64_t n = (*X).n_cols;
    
    switch (method)
    {
        case 0:
            // For fully-sampled ROSL
            InexactALM_ROSL(X);
            break;
        case 1:
            // For sub-sampled ROSL+
            arma::uvec rowall, colall;
            arma::arma_rng::set_seed(18931);
            rowall = (Sh == m) ? arma::linspace<arma::uvec>(0, m - 1, m) : arma::shuffle(
                    arma::linspace<arma::uvec>(0, m - 1, m));
            colall = (Sl == n) ? arma::linspace<arma::uvec>(0, n - 1, n) : arma::shuffle(
                    arma::linspace<arma::uvec>(0, n - 1, n));
            
            arma::uvec rowsample, colsample;
            rowsample = (Sh == m) ? rowall : arma::join_vert(rowall.subvec(0, Sh - 1),
                                                             arma::sort(rowall.subvec(Sh, m - 1)));
            colsample = (Sl == n) ? colall : arma::join_vert(colall.subvec(0, Sl - 1),
                                                             arma::sort(colall.subvec(Sl, n - 1)));
            
            arma::mat Xperm;
            Xperm = (*X).rows(rowsample);
            Xperm = Xperm.cols(colsample);
            
            // Take the columns and solve the small ROSL problem
            arma::mat XpermTmp;
            XpermTmp = Xperm.cols(0, Sl - 1);
            InexactALM_ROSL(&XpermTmp);
            
            // Free some memory
            XpermTmp.set_size(Sh, Sl);
            
            // Now take the rows and do robust linear regression
            XpermTmp = Xperm.rows(0, Sh - 1);
            InexactALM_RLR(&XpermTmp);
            
            // Free some memory
            Xperm.reset();
            XpermTmp.reset();
            
            // Calculate low-rank component
            A = D * alpha;
            
            // Permute back
            A.cols(colsample) = A;
            A.rows(rowsample) = A;
            
            // Calculate error
            E = *X - A;
            break;
    }
    
    // Free some memory
    Z.reset();
    Etmp.reset();
    error.reset();
    A.reset();
}

void ROSL::InexactALM_ROSL(arma::mat *X)
{
    uint64_t m = (*X).n_rows;
    uint64_t n = (*X).n_cols;
    int precision = (int)std::abs(std::log10(tol)) + 2;
    
    // Initialize A, Z, E, Etmp and error
    A.set_size(m, n);
    Z.set_size(m, n);
    E.set_size(m, n);
    Etmp.set_size(m, n);
    alpha.set_size(R, n);
    D.set_size(m, R);
    error.set_size(m, n);
    
    // Initialize alpha randomly
    arma::arma_rng::set_seed(18931);
    alpha.randu();
    
    // Set all other matrices
    A = *X;
    D.zeros();
    E.zeros();
    Z.zeros();
    Etmp.zeros();
    
    double infnorm, fronorm;
    infnorm = arma::norm(arma::vectorise(*X), "inf");
    fronorm = arma::norm(*X, "fro");
    
    // These are tunable parameters
    double rho, mubar;
    mu = 10 * lambda / infnorm;
    rho = 1.5;
    mubar = mu * 1E7;
    
    double stopcrit;
    
    for (uint64_t i = 0; i < maxIter; i++)
    {
        // Error matrix and intensity thresholding
        Etmp = *X + Z - A;
        E = arma::abs(Etmp) - lambda / mu;
        E.transform([](double val) { return (val > 0.) ? val : 0.; });
        E = E % arma::sign(Etmp);
        
        // Perform the shrinkage
        LowRankDictionaryShrinkage(X);
        
        // Update Z
        Z = (Z + *X - A - E) / rho;
        mu = (mu * rho < mubar) ? mu * rho : mubar;
        
        // Calculate stop criterion
        stopcrit = arma::norm(*X - A - E, "fro") / fronorm;
        roslIters = i + 1;
        
        // Exit if stop criteria is met
        if (stopcrit < tol)
        {
            // Report progress
            if (verbose)
            {
                std::cout << "---------------------------------------------------------" << std::endl;
                std::cout << "   ROSL iterations: " << i + 1 << std::endl;
                std::cout << "    Estimated rank: " << D.n_cols << std::endl;
                std::cout << "       Final error: " << std::fixed << std::setprecision(precision) << stopcrit
                          << std::endl;
                std::cout << "---------------------------------------------------------" << std::endl;
            }
            return;
        }
    }
    
    // Report convergence warning
    std::cout << "---------------------------------------------------------" << std::endl;
    std::cout << "   WARNING: ROSL did not converge in " << roslIters << " iterations" << std::endl;
    std::cout << "            Estimated rank:  " << D.n_cols << std::endl;
    std::cout << "               Final error: " << std::fixed << std::setprecision(precision) << stopcrit << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
}

void ROSL::InexactALM_RLR(arma::mat *X)
{
    uint64_t m = (*X).n_rows;
    uint64_t n = (*X).n_cols;
    int precision = (int)std::abs(std::log10(tol)) + 2;
    
    // Initialize A, Z, E, Etmp
    A.set_size(m, n);
    Z.set_size(m, n);
    E.set_size(m, n);
    Etmp.set_size(m, n);
    
    // Set all other matrices
    A = *X;
    E.zeros();
    Z.zeros();
    Etmp.zeros();
    
    double infnorm, fronorm;
    infnorm = arma::norm(arma::vectorise(*X), "inf");
    fronorm = arma::norm(*X, "fro");
    
    // These are tunable parameters
    double rho, mubar;
    mu = 10 * 5E-2 / infnorm;
    rho = 1.5;
    mubar = mu * 1E7;
    
    double stopcrit;
    
    for (uint64_t i = 0; i < maxIter; i++)
    {
        // Error matrix and intensity thresholding
        Etmp = *X + Z - A;
        E = arma::abs(Etmp) - 1 / mu;
        E.transform([](double val) { return (val > 0.) ? val : 0.; });
        E = E % arma::sign(Etmp);
        
        // SVD variables
        arma::mat Usvd, Vsvd;
        arma::vec Ssvd;
        arma::uvec Sshort;
        uint64_t SV;
        
        // Given D and A...
        arma::svd_econ(Usvd, Ssvd, Vsvd, D.rows(0, Sh - 1));
        Sshort = arma::find(Ssvd > 0.);
        SV = Sshort.n_elem;
        alpha = Vsvd.cols(0, SV - 1) * arma::diagmat(1. / Ssvd.subvec(0, SV - 1)) * arma::trans(Usvd.cols(0, SV - 1)) *
                (*X + Z - E);
        A = (D.rows(0, Sh - 1)) * alpha;
        
        // Update Z
        Z = (Z + *X - A - E) / rho;
        mu = (mu * rho < mubar) ? mu * rho : mubar;
        
        // Calculate stop criterion
        stopcrit = arma::norm(*X - A - E, "fro") / fronorm;
        rlrIters = i + 1;
        
        // Exit if stop criteria is met
        if (stopcrit < tol)
        {
            // Report progress
            if (verbose)
            {
                std::cout << "---------------------------------------------------------" << std::endl;
                std::cout << "    RLR iterations: " << i + 1 << std::endl;
                std::cout << "    Estimated rank: " << D.n_cols << std::endl;
                std::cout << "       Final error: " << std::fixed << std::setprecision(precision) << stopcrit
                          << std::endl;
                std::cout << "---------------------------------------------------------" << std::endl;
            }
            return;
        }
    }
    
    // Report convergence warning
    std::cout << "---------------------------------------------------------" << std::endl;
    std::cout << "   WARNING: RLR did not converge in " << rlrIters << " iterations" << std::endl;
    std::cout << "            Estimated rank:  " << D.n_cols << std::endl;
    std::cout << "               Final error: " << std::fixed << std::setprecision(precision) << stopcrit << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
}

void ROSL::LowRankDictionaryShrinkage(arma::mat *X)
{
    // Get current rank estimate
    rank = D.n_cols;
    
    // Thresholding
    double alphanormthresh;
    arma::vec alphanorm(rank);
    alphanorm.zeros();
    arma::uvec alphaindices;
    
    // Norms
    double dnorm;
    
    // Loop over columns of D
    for (uint64_t i = 0; i < rank; i++)
    {
        // Compute error and new D(:,i)
        D.col(i).zeros();
        error = ((*X + Z - E) - (D * alpha));
        D.col(i) = error * arma::trans(alpha.row(i));
        dnorm = arma::norm(D.col(i));
        
        // Shrinkage
        if (dnorm > 0.)
        {
            // Gram-Schmidt on D
            for (uint64_t j = 0; j < i; j++)
            {
                D.col(i) = D.col(i) - D.col(j) * (arma::trans(D.col(j)) * D.col(i));
            }
            
            // Normalize
            D.col(i) /= arma::norm(D.col(i));
            
            // Compute alpha(i,:)
            alpha.row(i) = arma::trans(D.col(i)) * error;
            
            // Magnitude thresholding
            alphanorm(i) = arma::norm(alpha.row(i));
            alphanormthresh = (alphanorm(i) - 1 / mu > 0.) ? alphanorm(i) - 1 / mu : 0.;
            alpha.row(i) *= alphanormthresh / alphanorm(i);
            alphanorm(i) = alphanormthresh;
        }
        else
        {
            alpha.row(i).zeros();
            alphanorm(i) = 0.;
        }
    }
    
    // Delete the zero bases
    alphaindices = arma::find(alphanorm != 0.);
    D = D.cols(alphaindices);
    alpha = alpha.rows(alphaindices);
    
    // Update A
    A = D * alpha;
}

} // namespace Algorithms
