//
// Created by zakhar on 13.03.18.
//

#include <armadillo>

#pragma once

namespace Stats
{

class CorrelationMatrix
{
    //
    // Data
    //
  private:
    arma::mat &matrix;
    arma::mat *correlation;
    
    std::vector<double> mean;
    std::vector<double> stddev;
    
    bool corrCalculated = false;
    
    //
    // Constructors & destructors
    //
  public:
    explicit CorrelationMatrix(arma::mat &mx);
    
    ~CorrelationMatrix();
    
    //
    // API
    //
  public:
    arma::mat &getCorrelationMatrix();
    
    arma::vec getSingularValuesOfCM();
    
    void normalizeMatrix();
    
    void deNormalizeMatrix();
    
    const std::vector<double> &getMean() const;
    const std::vector<double> &getStddev() const;
    //
    // Algorithm
    //
  private:
    void setMeanAndStdDev();
    
    double getCorrelation(uint64_t col1, uint64_t col2);
};

} // namespace Stats
