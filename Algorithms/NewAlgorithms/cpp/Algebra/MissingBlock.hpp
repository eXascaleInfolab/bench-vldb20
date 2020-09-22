//
// Created by zakhar on 05.03.18.
//

#pragma once

#include <armadillo>

namespace Algorithms
{

class MissingBlock
{
    //
    // Data
    //
  public:
    const uint64_t column;
    const uint64_t startingIndex;
    const uint64_t blockSize;
    
    arma::mat &matrix;
    
    //
    // Constructors & destructors
    //
  public:
    MissingBlock(uint64_t column, uint64_t startingIndex, uint64_t blockSize,
                 arma::mat &matrix)
            : column(column), startingIndex(startingIndex), blockSize(blockSize),
              matrix(matrix)
    { }
  
  public:
    arma::vec extractBlock()
    {
        arma::vec extraction(blockSize);
        
        for (uint64_t i = 0; i < blockSize; ++i)
        {
            extraction[i] = matrix(startingIndex + i, column);
        }
        
        return extraction;
    }
    
    void imputeBlock(const arma::vec &data)
    {
        for (uint64_t i = 0; i < blockSize; ++i)
        {
            matrix(column, startingIndex + i) = data.at(i);
        }
    }
};

} // namespace Algorithms
