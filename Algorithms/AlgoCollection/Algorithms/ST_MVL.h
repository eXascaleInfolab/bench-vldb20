//
// Created by zakhar on 04/01/19.
//

#pragma once

#include <map>
#include <array>
#include <armadillo>

// Conversion from C# summary:
// - int                       ==> uint64_t
// - Dictionary<TKey, TVavlue> ==> std::map<TKey, TValue>
// - List<T>                   ==> std::vector<T>
// - List<T> where T : const K ==> std::array<T>
// - double[,]                 ==> arma::mat

namespace Algorithms
{

class ST_MVL
{
  private:
    const std::string techFolder = "_technical/ST-MVL/";
    const std::string equationFile = techFolder + "stmvl_equation.txt";
    const std::string trainingFolder = techFolder + "TrainingFolder/";
  
  private:
    uint64_t rowCount;
    uint64_t columnCount;
    double alpha;
    double gamma;
    uint64_t windowSize;
    
    const double default_missing  = std::numeric_limits<double>::quiet_NaN();
    //const double default_missing  = 0.010101010101010101;
    
    arma::mat &missingMatrix;
    arma::mat predictMatrix;
    arma::mat temporaryMatrix;
    
    std::map<std::pair<uint64_t, uint64_t>, double> distanceDic;
    std::map<uint64_t, std::vector<uint64_t>> stationDic;
    
    const uint64_t temporal_threshold = 5;
    const static uint64_t viewCount = 4;
  
  public:
    
    // aux
    inline double ComputeSpatialWeight(double dis);
    
    inline double ComputeTemporalWeight(uint64_t timespan);
    
    // construct
    explicit ST_MVL(arma::mat &_missing, const std::string &latlong,
                    double _alpha, double _gamma,
                    uint64_t _windowSize);
    
    void Run(bool isBlockMissing);
    
    void doSTMVL();
    
    void InitializeMissing();
    
    void GlobalViewCombine(uint64_t i, uint64_t j);
    
    void MVL(uint64_t i, uint64_t j, arma::mat &equation);
    
    double UCF(uint64_t ti, uint64_t tj, arma::mat &dataMatrix);
    
    double calUserEuclideanSim(uint64_t tj, uint64_t jj, uint64_t ti, arma::mat &dataMatrix);
    
    double ICF(uint64_t ti, uint64_t tj, arma::mat &dataMatrix);
    
    double calItemEuclideanSim(uint64_t ti, uint64_t ii, uint64_t tj, arma::mat &dataMatrix);
    
    double SES(uint64_t ti, uint64_t tj, arma::mat &dataMatrix);
    
    double IDW(uint64_t ti, uint64_t tj, arma::mat &dataMatrix);
    
    void GenerateTrainingCase();
    
    uint64_t checkContextData(uint64_t ti, uint64_t tj);
    
    void outputCase(uint64_t i, uint64_t j, std::ofstream &swTrain);
    
    void FourView(uint64_t sensorCount);
    
    
    void sqt2(const arma::mat &x,
              const arma::vec &y, std::array<double, viewCount + 1> &a,
              std::array<double, 4> &dt, std::array<double, viewCount> &v);
    
    int chlk(arma::vec &a, uint64_t n, uint64_t m, std::array<double, viewCount + 1> &d);
    
    inline bool ismissing(double val);
    
    double GeoDistance(double lat1, double lng1, double lat2, double lng2);
    
    inline double RadToDeg(double d);
};

// Auxiliary


} // namespace Algorithms



