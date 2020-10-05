//
// Created on 09/01/19.
//

//
// Code translated to C++ from the original: https://www.cs.cmu.edu/afs/cs/project/spirit-1/www/#Download.
// The original code was adpated for recovery in https://www.ifi.uzh.ch/en/dbtg/Staff/wellenzohn/dasa.html
//

#include <iostream>

#include <cmath>
#include <numeric>
#include <algorithm>

#include "SPIRIT.h"

namespace Algorithms
{

void SPIRIT::doSpirit(arma::mat &A, uint64_t k0, uint64_t w, double lambda)
{
    //
    // Step 0: prep
    // impute last exising value instead of all NaNs
    //
    
    uint64_t blockStart = static_cast<unsigned>(-1);
    uint64_t blockEnd = static_cast<unsigned>(-1);
    double lastVal = 0;
    
    for (uint64_t i = 0; i < A.n_rows; ++i)
    {
        if (std::isnan(A.at(i,0)))
        {
            lastVal = A.at(i - 1, 0);
            blockStart = i;
            break;
        }
    }
    for (uint64_t i = blockStart; i < A.n_rows; ++i)
    {
        if (std::isnan(A.at(i,0)))
        {
            A.at(i, 0) = lastVal;
        }
        else
        {
            blockEnd = i;
            break;
        }
    }
    
    if (blockEnd == static_cast<unsigned>(-1))
    {
        blockEnd = A.n_rows - 1;
    }
    
    uint64_t n = A.n_cols;
    uint64_t totalTime = A.n_rows;
    
    arma::mat Proj(totalTime, n);
    arma::mat recon(totalTime, n);
    
    //initialize w_i to unit vectors
    arma::mat W = arma::eye<arma::mat>(n, n);
    arma::vec d(n);
    d.fill(0.01);
    //k0 = number of eigencomponents, passed as a param
    
    arma::vec relErrors(totalTime);
    
    arma::mat prevW(W);
    arma::mat Yvalues(totalTime, k0);
    arma::mat ARc = arma::zeros<arma::mat>(w, k0); //AR coefficients, one for each hidden variable
    std::vector<arma::mat> G;  //"Gain-Matrix", one for each hidden variable
    
    //initialize the "Gain-Matrix" with the identity matrix
    for (uint64_t j = 0; j < k0; ++j)
    {
        G.emplace_back(arma::zeros<arma::mat>(w, w));
        arma::mat &Gj = G[j];
        arma::diagview<double> diag = Gj.diag();
        diag.fill(1 / 0.004);
    }
    
    for (uint64_t t = 0; t < totalTime; ++t)
    {
        //Simulate a missing block
        if (blockStart <= t && t <= blockEnd)
        {
            //one-step forecast for each y-value
            arma::vec Y(k0);
            for (uint64_t j = 0; j < k0; ++j)
            {
                arma::vec xj = Yvalues.col(j).subvec(t - w, t - 1);
                arma::vec aj = ARc.col(j);
                Y[j] = arma::dot(xj, aj); //Eq 1 in Muscles paper
            }
            
            //estimate the missing value
            arma::vec xProj = prevW.submat(0, 0, prevW.n_rows - 1, k0 - 1) * Y; //reconstruction of the current time
            A.at(t, 0) = xProj[0]; //feed back imputed value
        }
        
        //update W for each y_t
        arma::vec x = A.row(t).t();
        arma::mat subW = W.submat(0, 0, W.n_rows - 1, k0 - 1);
        
        for (uint64_t j = 0; j < k0; ++j)
        {
            arma::vec Wj = subW.col(j);
            updateW(x, Wj, d[j], lambda);
            for (uint64_t i = 0; i < subW.n_rows; ++i)
            {
                subW(i, j) = Wj[i];
            }
        }
        
        grams(subW);
        
        for (uint64_t i = 0; i < subW.n_rows; ++i)
        {
            for (uint64_t j = 0; j < subW.n_cols; ++j)
            {
                W.at(i, j) = subW(i, j);
            }
        }
        
        //compute low-D projection, reconstruction and relative error
        arma::vec Y = subW.t() * A.row(t).t(); //project to m-dimensional space
        
        prevW = arma::mat(W);
        for (uint64_t i = 0; i < Y.n_rows; ++i)
        {
            Yvalues.at(t, i) = Y[i];
        }
    
        arma::vec xActual = A.row(t).t(); //actual vector of the current time
        arma::vec xProj = subW * Y; //reconstruction of the current time
    
        for (uint64_t i = 0; i < k0; ++i)
        {
            Proj.at(t, i) = Y[i];
        }
    
        for (uint64_t i = 0; i < xProj.n_rows; ++i)
        {
            recon.at(t, i) = xProj[i];
        }
        
        arma::vec xOrth = xActual - xProj;
        
        // relErrors(t) = sum(xOrth.^2)/sum(xActual.^2);
        relErrors[t] =
                std::accumulate(xOrth.begin(), xOrth.end(), 0.0,
                                [](const double &accu, const double &elem) { return accu + pow(elem, 2); })
                /
                std::accumulate(xActual.begin(), xActual.end(), 0.0,
                                [](const double &accu, const double &elem) { return accu + pow(elem, 2); });
        
        //update the AR coefficients for each hidden variable
        if (t >= w)
        {
            //we can start only when we have seen w measurements
            for (uint64_t j = 0; j < k0; ++j)
            {
                arma::vec xj = Yvalues.col(j).subvec(t - w + 1, t);// xj = Yvalues(t - w + 1:t, j)^T;
                double yj = Yvalues.at(t, j);
                arma::vec aj = ARc.col(j);
                arma::mat &Gj = G[j];
    
                arma::vec Gjxj = Gj * xj;
                arma::vec GjxjT = Gj.t() * xj;
    
                arma::mat X = Gjxj * GjxjT.t();
                
                // Gj = (1/lambda)*Gj - (1/lambda) *inv(lambda + xj' * Gj * xj) * (Gj * xj) * (xj' * Gj);
                Gj = (1 / lambda) * Gj - (1 / lambda) * (1 / (lambda + arma::dot(xj, Gjxj))) * X;
                
                Gjxj = Gj * xj; //recompute
                aj -= Gjxj * (arma::dot(xj, aj) - yj);
    
                for (uint64_t i = 0; i < aj.n_rows; ++i)
                {
                    ARc.at(i, j) = aj[i];
                }
            }
        }
        
    }
    
    // pull data in col 0, rows [range] from recon into A
    for (uint64_t i = blockStart; i <= blockEnd; ++i)
    {
        A.at(i, 0) = recon.at(i, 0);
    }
}

void SPIRIT::grams(arma::mat &A)
{
    uint64_t n = A.n_cols;
    
    for (uint64_t j = 1; j < n; ++j)
    {
        arma::vec Aj = A.col(j);
        
        for (uint64_t k = 0; k <= j - 1; ++k)
        {
            arma::vec Ak = A.col(k);
            double mult = arma::dot(Aj, Ak) / arma::dot(Ak, Ak);
            
            Aj -= (mult * Ak);
        }
        for (uint64_t i = 0; i < Aj.n_rows; ++i)
        {
            A.at(i, j) = Aj[i];
        }
    }
    
    for (uint64_t j = 0; j < n; ++j)
    {
        double norm2 = 0.0;
        for (uint64_t i = 0; i < A.n_rows; ++i)
        {
            norm2 += A.at(i, j) * A.at(i, j);
        }
        norm2 = sqrt(norm2);
        
        if (norm2 < sqrtEps)
        {
            throw std::runtime_error("[->grams] Columns of A are linearly dependent.");
        }
        else
        {
            for (uint64_t i = 0; i < A.n_rows; ++i)
            {
                A.at(i, j) /= norm2;
            }
        }
    }
}

void SPIRIT::updateW(arma::vec &old_x, arma::vec &old_w, double &d, double lambda)
{
    double y = arma::dot(old_w, old_x);
    d = lambda * d + pow(y, 2);
    arma::vec w(old_w);
    arma::vec e = old_x - (w * y);
    w = old_w + ((e * y) / d);
    old_w = arma::vec(w);
    arma::vec x = old_x - w * y;
    old_w = old_w / arma::norm(old_w);
    
    old_x = std::move(x);
}

//*/

} // namespace Algorithms
