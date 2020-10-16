//
// Created on 05.03.18.
//

//
// Code translated to C++ from the original: https://github.com/eXascaleInfolab/cdrec
//

#include "CDMissingValueRecovery.h"
#include "../Stats/Correlation.h"

#include <iostream>

namespace Algorithms
{

CDMissingValueRecovery::CDMissingValueRecovery(arma::mat &src,
                                               uint64_t maxIterations,
                                               double eps)
        : matrix(src),
          cd(src, src.n_cols - 1),
          k(src.n_cols - 1),
          cm(src),
          maxIterations(maxIterations), //defaults are hardcoded in CDMVR.h
          epsPrecision(eps),
          missingBlocks(std::vector<MissingBlock>()),
          disableCaching(false)
{ }

uint64_t CDMissingValueRecovery::getReduction()
{
    return k;
}

void CDMissingValueRecovery::setReduction(uint64_t k)
{
    this->k = k;
    cd.truncation = k;
}

void CDMissingValueRecovery::addMissingBlock(uint64_t col, uint64_t start, uint64_t size)
{
    missingBlocks.emplace_back(MissingBlock(col, start, size, matrix));
}

void CDMissingValueRecovery::addMissingBlock(MissingBlock mb)
{
    missingBlocks.emplace_back(mb);
}

void CDMissingValueRecovery::autoDetectMissingBlocks(double val)
{
    for (uint64_t j = 0; j < matrix.n_cols; ++j)
    {
        bool missingBlock = false;
        uint64_t start = 0;
        
        for (uint64_t i = 0; i < matrix.n_rows; ++i)
        {
            if ((std::isnan(val) && std::isnan(matrix.at(i, j))) || (!std::isnan(val) && matrix.at(i, j) == val))
            {
                if (!missingBlock)
                {
                    missingBlock = true;
                    start = i;
                }
            }
            else
            {
                if (missingBlock)
                {
                    //finalize block
                    missingBlock = false;
                    addMissingBlock(j, start, i - start);
                }
            }
        }
        
        if (missingBlock)
        {
            addMissingBlock(j, start, matrix.n_rows - start);
        }
    }
}

//
// Algorithm
//
void CDMissingValueRecovery::decomposeOnly()
{
    this->cd.performDecomposition(nullptr);
}

void CDMissingValueRecovery::increment(const std::vector<double> &vec)
{
    cd.increment(vec);
    
    if (useNormalization && false) // not implemented
    {
        uint64_t lastIdx = matrix.n_rows - 1;
        const std::vector<double> &mean = cm.getMean();
        const std::vector<double> &stddev = cm.getStddev();
        
        for (uint64_t j = 0; j < matrix.n_cols; ++j)
        {
            matrix.at(lastIdx, j) = (matrix.at(lastIdx, j) - mean[j]) / stddev[j];
        }
    }
}

void CDMissingValueRecovery::increment(const arma::vec &vec)
{
    cd.increment(vec);
    
    if (useNormalization && false) // not implemented
    {
        uint64_t lastIdx = matrix.n_rows - 1;
        const std::vector<double> &mean = cm.getMean();
        const std::vector<double> &stddev = cm.getStddev();
        
        for (uint64_t j = 0; j < matrix.n_cols; ++j)
        {
            matrix.at(lastIdx, j) = (matrix.at(lastIdx, j) - mean[j]) / stddev[j];
        }
    }
}

#define RECOVERY_VERBOSE_

uint64_t CDMissingValueRecovery::performRecovery(bool determineReduction /*= false*/)
{
    uint64_t totalMBSize = 0;
    
    for (auto mblock : missingBlocks)
    {
        totalMBSize += mblock.blockSize;
    }
    
    interpolate();
    
    uint64_t iter = 0;
    double delta = 99.0;
    
    if (useNormalization)
    {
        cm.normalizeMatrix();
    }
    
    if (determineReduction)
    {
        this->determineReduction();
    }
    
    auto centroidValues = std::vector<double>();
    
    while (++iter <= maxIterations && delta >= epsPrecision)
    {
        if (disableCaching)
        { cd.resetSignVectors(); }
        
        this->cd.performDecomposition(&centroidValues);
        
        const arma::mat &L = cd.getLoad();
        const arma::mat &R = cd.getRel();
        
        arma::mat recover = L * R.t();
        
        delta = 0.0;
        
        for (auto mblock : missingBlocks)
        {
            for (uint64_t i = mblock.startingIndex; i < mblock.startingIndex + mblock.blockSize; ++i)
            {
                double diff = matrix.at(i, mblock.column) - recover.at(i, mblock.column);
                delta += fabs(diff);
                
                matrix.at(i, mblock.column) = recover.at(i, mblock.column);
                recover.at(i, mblock.column) = 0.0;
            }
        }
        
        delta = delta / (double)totalMBSize;

#ifdef RECOVERY_VERBOSE
        lastrecon -= recover;
        delta = arma::norm(lastrecon) / std::sqrt((double)lastrecon.n_rows);
        lastrecon = recover;
        std::cout << "delta_" << iter << " =" << delta << std::endl << std::endl;
#endif
    }
    
    if (useNormalization)
    {
        cm.deNormalizeMatrix();
    }
    
    lastIterations = iter - 1;
    std::cout << "recovery performed in " << lastIterations << " iterations " << std::endl;
    
    // when the recovery is done, we need to clean up some stuff
    missingBlocks.clear();
    
    return iter - 1;
}

void CDMissingValueRecovery::interpolate()
{
    // init missing blocks
    for (auto mblock : missingBlocks)
    {
        // linear interpolation
        double val1 = NAN, val2 = NAN;
        if (mblock.startingIndex > 0)
        {
            val1 = matrix.at(mblock.startingIndex - 1, mblock.column);
        }
        if (mblock.startingIndex + mblock.blockSize < matrix.n_rows)
        {
            val2 = matrix.at(mblock.startingIndex + mblock.blockSize, mblock.column);
        }
        
        double step;
        
        // fallback case - no 2nd value for interpolation
        if (std::isnan(val1) && std::isnan(val2))
        {
            val1 = 0.0;
            step = 0;
        }
        else if (std::isnan(val1)) // start block is missing
        {
            val1 = val2;
            step = 0;
        }
        else if (std::isnan(val2)) // end block is missing
        {
            step = 0;
        }
        else
        {
            step = (val2 - val1) / (double)(mblock.blockSize + 1);
        }
        
        for (uint64_t i = 0; i < mblock.blockSize; ++i)
        {
            matrix.at(mblock.startingIndex + i, mblock.column) = val1 + step * (double)(i + 1);
        }
    }
}

void CDMissingValueRecovery::determineReduction()
{
    // step 1 - do full CD to determine rank
    
    std::vector<double> centroidValues = std::vector<double>();
    centroidValues.reserve(matrix.n_cols);
    cd.truncation = matrix.n_cols;
    cd.performDecomposition(&centroidValues, true);
    
    uint64_t rank = centroidValues.size();
    
    double squaresum = 0.0;
    
    std::cout << "CValues (rank=" << rank << "): ";
    for (auto &a : centroidValues)
    {
        a /= (double)matrix.n_rows;
        squaresum += a * a;
        std::cout << a << " ";
    }
    std::cout << std::endl;
    
    // step 2 [ALT] - entropy
    
    std::vector<double> relContribution = std::vector<double>();
    relContribution.reserve(rank);
    for (auto a : centroidValues)
    {
        relContribution.emplace_back(a * a / squaresum);
    }
    
    double entropy = 0.0;
    for (auto a : relContribution)
    {
        entropy += a * std::log(a);
    }
    entropy /= -std::log(rank);
    
    uint64_t red;
    double contributionSum = relContribution[0];
    for (red = 1; red < rank - 1; ++red)
    {
        if (contributionSum >= entropy)
        { break; }
        contributionSum += relContribution[red];
    }
    
    std::cout << "Auto-reduction [entropy] detected as: "
              << red << " in [1..." << rank - 1 << "]," << std::endl
              << "with  sum(contrib)=" << contributionSum
              << " entropy=" << entropy << std::endl << std::endl;
    
    // cleanup - we will have less dimensions later
    cd.destroyDecomposition();
    
    setReduction(red);
}

// simplistic API
void CDMissingValueRecovery::RecoverMatrix(arma::mat &matrix, uint64_t k, double eps)
{
    CDMissingValueRecovery rmv(matrix, 100, eps);
    
    rmv.setReduction(k);
    rmv.autoDetectMissingBlocks();
    rmv.performRecovery(k == 0);
}


} // namespace Algorithms
