//
// Created by Zakhar on 08.03.2017.
//

#include <armadillo>

#pragma once

namespace Algorithms
{

class CentroidDecomposition
{
    //
    // Data
    //
  private:
    arma::mat &Src;
    arma::mat Load;
    arma::mat Rel;
  
  public:
    std::vector<arma::vec> signVectors;
    //std::vector<std::vector<arma::vec *>> signVectorSteps;
    uint64_t ssvIterations = 0;
    uint64_t truncation = 0;
    
    //
    // Constructors & destructors
    //
  public:
    explicit CentroidDecomposition(arma::mat &mx);
    
    CentroidDecomposition(arma::mat &mx, uint64_t k);
    
    ~CentroidDecomposition() = default;
    
    //
    // API
    //
  public:
    const arma::mat &getLoad();
    
    const arma::mat &getRel();
    
    arma::mat stealLoad();
    
    arma::mat stealRel();
    
    void destroyDecomposition();
    
    void resetSignVectors();
    
    void performDecomposition(std::vector<double> *centroidValues = nullptr,
                              bool stopOnIncompleteRank = false, bool skipSSV = false);
    
    void increment(const arma::vec &vec);
    
    void increment(const std::vector<double> &vec);
    
    //
    // Algorithm
    //
  private:
    bool decomposed = false;
    uint64_t addedRows = 0;
    
    arma::vec &findLocalSignVector(arma::mat &mx, uint64_t k, bool useInit);
  
    //
    // Static
    //
  public:
    static constexpr double eps = 1E-11;
    
    static constexpr uint64_t minusone = static_cast<uint64_t>(-1);
    
    static std::pair<arma::mat, arma::mat> PerformCentroidDecomposition(arma::mat &mx, uint64_t k = 0);
};

} // namespace Algorithms
