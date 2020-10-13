//
// Created by Zakhar on 08.03.2017.
//

#include <cmath>
#include <iostream>
#include "CentroidDecomposition.h"
#include "Auxiliary.h"

namespace Algorithms
{
//
// CentroidDecomposition constructors & desctructors
//

CentroidDecomposition::CentroidDecomposition(arma::mat &mx)
        : Src(mx),
          Load(mx.n_rows, mx.n_cols),
          Rel(mx.n_cols, mx.n_cols),
          signVectors(std::vector<arma::vec>()),
          truncation(mx.n_cols)
{
    Load.zeros();
    Rel.zeros();
    
    // i = 0:
    arma::vec signV(mx.n_rows);
    signV.fill(1.0);
    signVectors.emplace_back(std::move(signV));
    
    // i in [1,m[
    for (uint64_t i = 1; i < mx.n_cols; ++i)
    {
        signVectors.emplace_back(signVectors[0]);
    }
}

CentroidDecomposition::CentroidDecomposition(arma::mat &mx, uint64_t k)
        : CentroidDecomposition(mx)
{
    truncation = k;
}

//
// CentroidDecomposition API
//

const arma::mat &CentroidDecomposition::getLoad()
{
    return Load;
}

const arma::mat &CentroidDecomposition::getRel()
{
    return Rel;
}

arma::mat CentroidDecomposition::stealLoad()
{
    return std::move(Load);
}

arma::mat CentroidDecomposition::stealRel()
{
    return std::move(Rel);
}

void CentroidDecomposition::destroyDecomposition()
{
    Load.zeros();
    Rel.zeros();
}

void CentroidDecomposition::resetSignVectors()
{
    for (uint64_t i = 0; i < Src.n_cols; ++i)
    {
        for (uint64_t j = 0; j < Src.n_rows; j++)
        {
            signVectors[i][j] = 1.0;
        }
    }
}

void CentroidDecomposition::performDecomposition(std::vector<double> *centroidValues,
                                                 bool stopOnIncompleteRank /*= false*/, bool skipSSV /*= false*/)
{
    arma::mat X(Src);
    
    for (uint64_t i = 0; i < truncation; i++)
    {
        arma::vec &Z = skipSSV
                    ? signVectors[i]
                    : findLocalSignVector(X, i, true);
        
        //std::cout << Z.toString() << std::endl;
        
        // C_*i = X^T * Z
        arma::vec Rel_i = X.t() * Z;
        
        // R_*i = C_*i / ||C_*i||
        double centroid = arma::norm(Rel_i);
        if (centroidValues != nullptr)
        {
            if (stopOnIncompleteRank && centroid < eps) // incomplete rank, don't even proceed with current iteration
            {
                break;
            }
            
            centroidValues->emplace_back(centroid);
        }
        
        Rel_i /= centroid;
        
        // R = Append(R, R_*i)
        Algebra::Operations::insert_vector_at_column(Rel, i, Rel_i);
        
        // L_*i = X * R
        arma::vec Load_i = X * Rel_i;
        
        // L = Append(L, L_*i)
        Algebra::Operations::insert_vector_at_column(Load, i, Load_i);
        
        // X := X - L_*i * R_*i^T
        X -= (Load_i * Rel_i.t());
    }
    
    addedRows = 0;
    decomposed = true;
}

//
// CentroidDecomposition algorithm
//

void CentroidDecomposition::increment(const arma::vec &vec)
{
    Algebra::Operations::increment_matrix(Src, vec);
    Algebra::Operations::increment_matrix(Load, vec); // doesn't matter, will be overwritten
    ++addedRows;
    
    for (uint64_t i = 0; i < Src.n_cols; ++i)
    {
        Algebra::Operations::increment_vector(signVectors[i], 1.0);
    }
}

void CentroidDecomposition::increment(const std::vector<double> &vec)
{
    increment(arma::vec(vec));
}

//
// Algorithm
//

arma::vec &CentroidDecomposition::findLocalSignVector(arma::mat &mx, uint64_t k, bool useInit)
{
    arma::vec &Z = signVectors[k]; // get a reference
    arma::vec direction;
    
    //
    // First pass - init
    //
    if (!decomposed && useInit)
    {
        direction = arma::vec(mx.n_cols);
        
        for (uint64_t j = 0; j < mx.n_cols; ++j)
        {
            direction[j] = mx.at(0, j);
        }
        
        for (uint64_t i = 1; i < mx.n_rows; ++i)
        {
            double gradPlus = 0.0;
            double gradMinus = 0.0;
            
            for (uint64_t j = 0; j < mx.n_cols; ++j)
            {
                double localModPlus = direction[j] + mx.at(i, j);
                gradPlus += localModPlus * localModPlus;
                double localModMinus = direction[j] - mx.at(i, j);
                gradMinus += localModMinus * localModMinus;
            }
            
            // if keeping +1 as a sign yields a net negative to the
            Z[i] = gradPlus > gradMinus ? 1 : -1;
            
            for (uint64_t j = 0; j < mx.n_cols; ++j)
            {
                direction[j] += Z[i] * mx.at(i, j);
            }
        }
    }
    else // Alternative first pass - init to {+1}^n
    {
        direction = (mx.t() * Z);
    }
    
    //
    // 2+ pass - update to Z
    //
    
    bool flipped;
    double lastNorm = // cache the current value of (||D||_2)^2 to avoid recalcs
            arma::dot(direction, direction) + eps; // eps to avoid "parity flip"
    
    do
    {
        flipped = false;
        
        for (uint64_t i = 0; i < mx.n_rows; ++i)
        {
            double signDouble = Z[i] * 2;
            double gradFlip = 0.0;
            
            for (uint64_t j = 0; j < mx.n_cols; ++j)
            {
                double localMod = direction[j] - signDouble * mx.at(i, j);
                gradFlip += localMod * localMod;
            }
            
            if (gradFlip > lastNorm) // net positive from flipping
            {
                flipped = true;
                Z[i] *= -1;
                lastNorm = gradFlip + eps;
                
                for (uint64_t j = 0; j < mx.n_cols; ++j)
                {
                    direction[j] -= signDouble * mx.at(i, j);
                }
            }
        }
    } while (flipped);
    
    return Z;
}


// simplistic API
std::pair<arma::mat, arma::mat>
CentroidDecomposition::PerformCentroidDecomposition(arma::mat &mx, uint64_t k)
{
    k = k == 0 ? mx.n_cols : k;
    CentroidDecomposition cd(mx);
    
    cd.truncation = k;
    
    cd.performDecomposition(nullptr);
    
    return std::make_pair(cd.stealLoad(), cd.stealRel());
}

} // namespace Algorithms
