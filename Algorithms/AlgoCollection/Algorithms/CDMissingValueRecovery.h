//
// Created by zakhar on 05.03.18.
//

#pragma once

#include <cmath>

#include "../Algebra/CentroidDecomposition.h"
#include "../Algebra/MissingBlock.hpp"
#include "../Stats/Correlation.h"

namespace Algorithms
{

class CDMissingValueRecovery
{
    //
    // Data
    //
  private:
    arma::mat &matrix;
    CentroidDecomposition cd;
    uint64_t k;
    Stats::CorrelationMatrix cm;
  
  public:
    const uint64_t maxIterations;
    uint64_t lastIterations = 0;
    double epsPrecision;
    std::vector<MissingBlock> missingBlocks;
    
    uint64_t optimization = 0;
    bool disableCaching = false;
    bool useNormalization = false;
    
    //
    // Constructors & desctructors
    //
  public:
    explicit CDMissingValueRecovery(arma::mat &src, uint64_t maxIterations = 100, double eps = 1E-6);
    
    //
    // API
    //
  public:
    uint64_t getReduction();
    
    void setReduction(uint64_t k);
    
    void passSignVectorStrategy(CDSignVectorStrategy_2 strategy);
    
    void addMissingBlock(uint64_t col, uint64_t start, uint64_t size);
    
    void addMissingBlock(MissingBlock mb);
    
    void autoDetectMissingBlocks(double val = NAN);
    
    void decomposeOnly();
    
    void increment(const std::vector<double> &vec);
    
    void increment(const arma::vec &vec);
    
    uint64_t performRecovery(bool determineReduction = false);
    
    //
    // Algorithm
    //
  private:
    void interpolate();
    
    void init_zero();
    
    void init_mean();
    
    void init_mean_horiz();
    
    void init_1NN();
    
    void determineReduction();
    
    //
    // Static
    //
  public:
    static void RecoverMatrix(arma::mat &matrix, uint64_t k = 0, double eps = 1E-6);
};
} // namespace Algorithms
