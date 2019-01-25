//
// Created by zakhar on 20/01/19.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

class ROSL
{
  public:
    static void ROSL_Recovery(arma::mat &input, uint64_t rank, double reg);
  
  public:
    explicit ROSL()
    {
        // Initialize default class parameters
        method = 0;
        Sl = 100;
        Sh = 100;
        R = 5;
        lambda = 0.02;
        tol = 1E-6;
        maxIter = 100;
        verbose = false;
    };
    
    ~ROSL()
    {
        // Clear memory
        D.reset();
        E.reset();
        A.reset();
        alpha.reset();
        Z.reset();
        Etmp.reset();
        error.reset();
    };
    
    // Full ROSL for data matrix X
    void runROSL(arma::mat *X);
    
    // Set parameters
    void Parameters(uint64_t rankEstimate, double lambdaParameter, double tolerance, uint64_t maxiterations,
                    uint64_t usermethod, uint64_t subsamplingl, uint64_t subsamplingh, bool verb)
    {
        method = usermethod;
        Sl = subsamplingl;
        Sh = subsamplingh;
        R = rankEstimate;
        lambda = lambdaParameter;
        tol = tolerance;
        maxIter = maxiterations;
        verbose = verb;
    };
    
    void getD(double *dPy, uint64_t m, uint64_t n)
    {
        D.resize(m, n);
        memcpy(dPy, D.memptr(), D.n_elem * sizeof(double));
        D.reset();
    };
    
    void getAlpha(double *alphaPy, uint64_t m, uint64_t n)
    {
        alpha.resize(m, n);
        memcpy(alphaPy, alpha.memptr(), alpha.n_elem * sizeof(double));
        alpha.reset();
    };
    
    void getE(double *ePy)
    {
        memcpy(ePy, E.memptr(), E.n_elem * sizeof(double));
        E.reset();
    };
    
    uint64_t getR()
    {
        return D.n_cols;
    }
  
  private:
    // Solve full ROSL via inexact Augmented Lagrangian Multiplier method
    void InexactALM_ROSL(arma::mat *X);
    
    // Robust linear regression for ROSL+ via inexact Augmented Lagrangian Multiplier method
    void InexactALM_RLR(arma::mat *X);
    
    // Dictionary shrinkage for full ROSL
    void LowRankDictionaryShrinkage(arma::mat *X);
    
    // User parameters
    uint64_t method, R, Sl, Sh, maxIter;
    double lambda, tol;
    bool verbose;
    
    // Basic parameters
    uint64_t rank, roslIters, rlrIters;
    double mu;
    
    // Armadillo matrices
    arma::mat D, A, E, alpha, Z, Etmp, error;
};

} // namespace Algorithms



