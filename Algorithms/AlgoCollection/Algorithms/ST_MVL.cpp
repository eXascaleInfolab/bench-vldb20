//
// Created by zakhar on 04/01/19.
//

#include <iostream>
#include <fstream>
#include <iomanip>

#include <cmath>
#include <numeric>
#include <algorithm>

#include "ST_MVL.h"

const double EARTH_RADIUS = 6378137.0; //earth radius

namespace Algorithms
{

inline double ST_MVL::ComputeSpatialWeight(double dis)
{
    double ws = pow((1 / dis), alpha);
    return ws;
}

inline double ST_MVL::ComputeTemporalWeight(uint64_t timespan)
{
    double wt = gamma * pow((1 - gamma), (double)(timespan - 1));
    return wt;
}

// construction

ST_MVL::ST_MVL(arma::mat &_missing, const std::string &latlong,
               double _alpha, double _gamma,
               uint64_t _windowSize
)
        : rowCount(_missing.n_rows), columnCount(_missing.n_cols),
          alpha(_alpha), gamma(_gamma),
          windowSize(_windowSize),
          missingMatrix(_missing),
          predictMatrix(_missing),
          temporaryMatrix(_missing)
{
    //
    // Step 1 : lat-long
    //
    std::vector<double> latitude;
    std::vector<double> longitude;
    
    std::ifstream latlongFile;
    latlongFile.open(latlong, std::ios::in);
    
    std::string buffer;
    std::getline(latlongFile, buffer); //drop header line
    
    while (latlongFile.peek() != EOF)
    {
        std::getline(latlongFile, buffer, ',');//drop station #
        std::getline(latlongFile, buffer, ',');//lat
        latitude.emplace_back(std::stod(buffer));
        std::getline(latlongFile, buffer);//long
        longitude.emplace_back(std::stod(buffer));
    }
    latlongFile.close();
    
    //
    // Step 2 : process distances
    //
    
    // was Dictionary<>, but it didn't have lookups by key and was only used to sort and iterate over
    std::vector<std::pair<int, double>> distance_dic_temp;
    
    for (uint64_t i = 0; i < columnCount; i++)
    {
        for (uint64_t j = 0; j < columnCount; j++)
        {
            if (i != j)
            {
                double dis = GeoDistance(latitude[i], longitude[i], latitude[j], longitude[j]);
                distanceDic.insert(std::make_pair(std::make_pair(i, j), dis));
                distance_dic_temp.emplace_back(std::make_pair(j, dis));
            }
        }
        
        // statement removed because in the original code it has no effect
        //std::sort(distance_dic_temp.begin(), distance_dic_temp.end(),
        //          [](std::pair<int, double> &p1, std::pair<int, double> &p2) { return p1.second < p2.second; });
        
        stationDic.insert(std::make_pair(i, std::vector<uint64_t>()));
        
        for (auto &p : distance_dic_temp)
        {
            stationDic[i].emplace_back(p.first);
        }
        
        distance_dic_temp.clear();
    }
}

void ST_MVL::Run(bool isBlockMissing)
{
    if (isBlockMissing)
    {
        InitializeMissing();
    }
    
    GenerateTrainingCase();
    
    FourView(columnCount);
    
    doSTMVL();
}

void ST_MVL::doSTMVL()
{
    std::cout << "Do STMVL... " << std::endl;
    
    std::string buffer;
    arma::mat equation(columnCount, 5); // 5 means 4 four views and residual
    
    std::ifstream srEquation;
    srEquation.open(equationFile, std::ios::in);
    
    uint64_t count = 0;
    
    while (srEquation.peek() != EOF)
    {
        for (uint64_t i = 0; i < 4; ++i)
        {
            std::getline(srEquation, buffer, ',');
            equation(count, i) = std::stod(buffer);
        }
        // i=5
        {
            std::getline(srEquation, buffer);
            equation(count, 4) = std::stod(buffer);
        }
        // tick
        count++;
    }
    srEquation.close();
    
    for (uint64_t i = 0; i < rowCount; i++)
    {
        for (uint64_t j = 0; j < columnCount; j++)
        {
            if (ismissing(missingMatrix.at(i, j)))
            {
                MVL(i, j, equation);
            }
        }
    }
    
    missingMatrix = std::move(predictMatrix);
}

void ST_MVL::InitializeMissing()
{
    std::cout << "Initialize Missing... " << std::endl;
    
    for (uint64_t i = 0; i < rowCount; i++)
    {
        for (uint64_t j = 0; j < columnCount; j++)
        {
            if (ismissing(missingMatrix.at(i, j)))
            {
                GlobalViewCombine(i, j);
            }
        }
    }
}

void ST_MVL::GlobalViewCombine(uint64_t i, uint64_t j)
{
    double result_IDW = IDW(i, j, missingMatrix);
    double result_SES = SES(i, j, missingMatrix);
    
    if (!ismissing(result_IDW) && !ismissing(result_SES))
    {
        temporaryMatrix.at(i, j) = (result_IDW + result_SES) / 2; // simple combine equally.
    }
    else if (!ismissing(result_SES))
    {
        temporaryMatrix.at(i, j) = result_SES;
    }
    else if (!ismissing(result_IDW))
    {
        temporaryMatrix.at(i, j) = result_IDW;
    }
}

void ST_MVL::MVL(uint64_t i, uint64_t j, arma::mat &equation)
{
    double result_UCF = UCF(i, j, temporaryMatrix);
    double result_ICF = ICF(i, j, temporaryMatrix);
    double result_IDW = IDW(i, j, temporaryMatrix);
    double result_SES = SES(i, j, temporaryMatrix);
    
    if (!ismissing(result_UCF) && !ismissing(result_ICF) && !ismissing(result_IDW) && !ismissing(result_SES))
    {
        double result = equation.at(j, 0) * result_UCF + equation.at(j, 1) * result_IDW + equation.at(j, 2) * result_ICF +
                        equation.at(j, 3) * result_SES + equation.at(j, 4);
        predictMatrix.at(i, j) = result;
    }
    else
    {
        predictMatrix.at(i, j) = temporaryMatrix.at(i, j);
    }
}

double ST_MVL::UCF(uint64_t ti, uint64_t tj, arma::mat &dataMatrix)
{
    std::map<double, double> candiDic;
    
    for (auto jj : stationDic[tj])
    {
        if (!ismissing(dataMatrix.at(ti, jj)))
        {
            double sim = calUserEuclideanSim(tj, jj, ti, dataMatrix);
            
            if (sim != 0)
            {
                auto it = candiDic.find(dataMatrix.at(ti, jj));
                if (it != candiDic.end())
                {
                    it->second += sim;
                }
                else
                {
                    candiDic.insert(std::make_pair(dataMatrix.at(ti, jj), sim));
                }
            }
        }
    }
    
    double spatialPredict = default_missing;
    
    if (!candiDic.empty())
    {
        double weightSum = std::accumulate(
                candiDic.begin(), candiDic.end(), 0.0,
                [](double acc, const std::pair<double, double> &p2) {
                    return acc + p2.second;
                }
        );
        double tempSpatialPredict = 0;
        for (auto p : candiDic)
        {
            tempSpatialPredict += p.first * p.second / weightSum;
        }
        spatialPredict = tempSpatialPredict;
    }
    
    return spatialPredict;
}

double ST_MVL::calUserEuclideanSim(uint64_t tj, uint64_t jj, uint64_t ti, arma::mat &dataMatrix)
{
    double similarity = 0;
    double offset = 0;
    uint64_t NT = 0;
    
    uint64_t halfWindow = windowSize / 2;
    
    uint64_t upRow = ti >= halfWindow ? ti - halfWindow : 0;
    uint64_t downRow = ti + halfWindow;
    if (downRow >= rowCount)
    {
        downRow = rowCount - 1;
    }
    
    for (uint64_t i = upRow; i < downRow; i++)
    {
        if (!ismissing(dataMatrix.at(i, tj)) && !ismissing(dataMatrix.at(i, jj)))
        {
            offset += pow((dataMatrix.at(i, tj) - dataMatrix.at(i, jj)), 2);
            NT++;
        }
    }
    
    if (NT > 0 && offset > 0)
    {
        double avgDis = sqrt(offset) / (double) NT;
        similarity = 1 / (avgDis);
    }
    
    return similarity;
}

double ST_MVL::ICF(uint64_t ti, uint64_t tj, arma::mat &dataMatrix)
{
    std::map<double, double> candiDic;
    
    uint64_t halfWindow = windowSize / 2;
    
    uint64_t upRow = ti >= halfWindow ? ti - halfWindow : 0;
    uint64_t downRow = ti + halfWindow;
    if (downRow >= rowCount)
    {
        downRow = rowCount - 1;
    }
    
    for (uint64_t ii = upRow; ii < downRow; ii++)
    {
        if (ii == ti)
        {
            continue;
        }
        
        if (!ismissing(dataMatrix.at(ii, tj)))
        {
            double sim = calItemEuclideanSim(ti, ii, tj, dataMatrix);
            if (sim != 0)
            {
                auto it = candiDic.find(dataMatrix.at(ii, tj));
                if (it != candiDic.end())
                {
                    it->second += sim;
                }
                else
                {
                    candiDic.insert(std::make_pair(dataMatrix.at(ii, tj), sim));
                }
            }
        }
    }
    
    double temporalPredict = default_missing;
    if (!candiDic.empty())
    {
        double weightSum = std::accumulate(
                candiDic.begin(), candiDic.end(), 0.0,
                [](double acc, const std::pair<double, double> &p2) {
                    return acc + p2.second;
                }
        );
        double tempTemporalPredict = 0;
        for (auto p : candiDic)
        {
            tempTemporalPredict += p.first * p.second / weightSum;
        }
        temporalPredict = tempTemporalPredict;
    }
    
    return temporalPredict;
}

double ST_MVL::calItemEuclideanSim(uint64_t ti, uint64_t ii, uint64_t tj, arma::mat &dataMatrix)
{
    double similarity = 0;
    double offset = 0;
    uint64_t NS = 0;
    
    for (auto &jj : stationDic[tj])
    {
        if (!ismissing(dataMatrix.at(ti, jj)) && !ismissing(dataMatrix.at(ii, jj)))
        {
            offset += pow((dataMatrix.at(ti, jj) - dataMatrix.at(ii, jj)), 2);
            NS++;
        }
    }
    
    if (NS > 0 && offset > 0)
    {
        double avgDis = sqrt(offset) / (double)NS;
        similarity = 1 / (avgDis);
    }
    
    return similarity;
}

double ST_MVL::SES(uint64_t ti, uint64_t tj, arma::mat &dataMatrix)
{
    std::map<double, double> candiDic;
    
    for (uint64_t i = 1; i <= temporal_threshold; i++)
    {
        uint64_t ii = ti >= i ? ti - i : 0;
        if (ti >= i && !ismissing(dataMatrix.at(ii, tj)))
        {
            double weight = ComputeTemporalWeight(i);
            double value = dataMatrix.at(ii, tj);
            
            auto it = candiDic.find(value);
            if (it != candiDic.end())
            {
                it->second += weight;
            }
            else
            {
                candiDic.insert(std::make_pair(value, weight));
            }
        }
        
        ii = ti + i;
        if (ii < rowCount && !ismissing(dataMatrix.at(ii, tj)))
        {
            double weight = ComputeTemporalWeight(i);
            double value = dataMatrix.at(ii, tj);
            
            auto it = candiDic.find(value);
            if (it != candiDic.end())
            {
                it->second += weight;
            }
            else
            {
                candiDic.insert(std::make_pair(value, weight));
            }
        }
    }
    
    std::vector<std::pair<double, double>> candiVec(candiDic.begin(), candiDic.end());
    
    // statement removed because in the original code it has no effect
    //std::sort(candiVec.begin(), candiVec.end(), [](std::pair<double, double> &p1, std::pair<double, double> &p2) {
    //    return p1.second > p2.second; /*desc*/ });
    
    double temporalPredict = default_missing;
    if (!candiVec.empty())
    {
        double weightSum = std::accumulate(
                candiVec.begin(), candiVec.end(), 0.0,
                [](double acc, const std::pair<double, double> &p2) {
                    return acc + p2.second;
                }
        );
        double tempTemporalPredict = 0;
        for (auto &q : candiVec)
        {
            tempTemporalPredict += q.first * q.second / weightSum;
        }
        temporalPredict = tempTemporalPredict;
    }
    return temporalPredict;
}

double ST_MVL::IDW(uint64_t ti, uint64_t tj, arma::mat &dataMatrix)
{
    std::map<double, double> candiDic;
    
    for (auto &jj : stationDic[tj])
    {
        double dis = distanceDic[std::make_pair(tj, jj)];
        if (!ismissing(dataMatrix.at(ti, jj)))
        {
            double weight = ComputeSpatialWeight(dis);
            double value = dataMatrix.at(ti, jj);
            
            auto it = candiDic.find(value);
            if (it != candiDic.end())
            {
                it->second += weight;
            }
            else
            {
                candiDic.insert(std::make_pair(value, weight));
            }
        }
    }
    
    std::vector<std::pair<double, double>> candiVec(candiDic.begin(), candiDic.end());
    
    // statement removed because in the original code it has no effect
    //std::sort(candiVec.begin(), candiVec.end(), [](std::pair<double, double> &p1, std::pair<double, double> &p2) {
    //    return p1.second > p2.second; /*order by desc*/ });
    
    double spatialPredict = default_missing;
    if (!candiVec.empty())
    {
        double weightSum = std::accumulate(
                candiVec.begin(), candiVec.end(), 0.0,
                [](double acc, const std::pair<double, double> &p2) {
                    return acc + p2.second;
                }
        );
        double tempSpatialPredict = 0;
        for (auto q : candiVec)
        {
            tempSpatialPredict += q.first * q.second / weightSum;
        }
        spatialPredict = tempSpatialPredict;
    }
    
    return spatialPredict;
}

void ST_MVL::GenerateTrainingCase()
{
    std::cout << "Generate Training Case... " << std::endl;
    
    for (uint64_t j = 0; j < columnCount; j++)
    {
        uint64_t caseCount = 0;
        
        for (uint64_t i = 0; i < rowCount; i++)
        {
            if (!ismissing(missingMatrix.at(i, j)))
            {
                if (checkContextData(i, j) == 1)
                {
                    caseCount++;
                }
            }
        }
        
        std::ofstream swTrain;
        swTrain.open(trainingFolder + "train_" + std::to_string(j) + ".txt", std::ios::out);
        swTrain << caseCount << std::endl;
        swTrain << std::setprecision(15);//appx. matches C#
        
        for (uint64_t i = 0; i < rowCount; i++)
        {
            if (!ismissing(missingMatrix.at(i, j)))
            {
                if (checkContextData(i, j) == 1)
                {
                    outputCase(i, j, swTrain);
                }
            }
        }
        
        swTrain.close();
    }
}

uint64_t ST_MVL::checkContextData(uint64_t ti, uint64_t tj)
{
    uint64_t count = 0;
    for (uint64_t j = 0; j < columnCount; j++)
    {
        if (ismissing(missingMatrix.at(ti, j)))
        {
            count++;
        }
    }
    if (count > (columnCount / 2))
    {
        return 0;
    }
    
    uint64_t halfWindow = windowSize / 2;
    
    uint64_t si = ti >= halfWindow ? ti - halfWindow : 0;
    uint64_t ei = ti + halfWindow;
    if (ei >= rowCount)
    {
        ei = rowCount - 1;
    }
    
    count = 0;
    
    for (uint64_t i = si; i <= ei; i++)
    {
        if (ismissing(missingMatrix.at(i, tj)))
        {
            count++;
        }
    }
    
    if (count > ((ei - si + 1) / 2))
    {
        return 0;
    }
    
    return 1;
}

void ST_MVL::outputCase(uint64_t i, uint64_t j, std::ofstream &swTrain)
{
    double result_UCF = UCF(i, j, temporaryMatrix);
    double result_ICF = ICF(i, j, temporaryMatrix);
    double result_IDW = IDW(i, j, temporaryMatrix);
    double result_SES = SES(i, j, temporaryMatrix);
    
    if (!ismissing(result_UCF) && !ismissing(result_ICF) && !ismissing(result_IDW) && !ismissing(result_SES))
    {
        swTrain << missingMatrix.at(i, j) << "," << result_UCF << "," << result_IDW << "," << result_ICF << ","
                << result_SES << std::endl;
    }
}

void ST_MVL::FourView(uint64_t sensorCount)
{
    // local const
    
    std::cout << "Run Square Error..." << std::endl;
    
    std::ofstream sw;
    sw.open(equationFile, std::ios::out);
    sw << std::setprecision(15);//appx. matches C#
    //std::cout << "training error(MAE): " << std::endl;
    
    for (uint64_t j = 0; j < sensorCount; j++)
    {
        std::ifstream sr;
        sr.open(trainingFolder + "train_" + std::to_string(j) + ".txt", std::ios::in);
        std::string buffer;
        uint64_t count = 0;
        
        std::getline(sr, buffer);
        uint64_t rowNum = std::stoull(buffer);
        
        std::array<double, viewCount + 1> a{};
        std::array<double, viewCount> v{};
        std::array<double, 4> dt{};
        arma::mat x(viewCount, rowNum);
        arma::vec y(rowNum);
        
        while (sr.peek() != EOF)
        {
            std::getline(sr, buffer);
            
            const std::string separator = ",";
            
            size_t pos = 0;
            
            // i = -1
            {
                size_t newpos = buffer.find(separator, pos + 1);
                y[count] = std::stod(buffer.substr(pos, newpos - pos));
                pos = newpos + 1;
            }
            
            uint64_t i = 0;
            while (pos < buffer.size())
            {
                size_t newpos;
                newpos = buffer.find(separator, pos + 1);
                newpos = newpos == std::string::npos ? buffer.length() : newpos;
                
                std::string temp = buffer.substr(pos, newpos - pos);
                
                if (temp.empty())
                { break; }
                
                x(i, count) = std::stod(temp);
                
                pos = newpos + 1;
                
                i++;
            }
            
            count++;
        }
        sr.close();
        sqt2(x, y, a, dt, v);
        
        sw << a[0] << "," << a[1] << "," << a[2] << "," << a[3] << "," << a[4] << std::endl;
        
        double error = 0;
        for (uint64_t i = 0; i < rowNum; i++)
        {
            double vvv = a[0] * x(0, i) + a[1] * x(1, i) + a[2] * x(2, i) + a[3] * x(3, i) + a[4];
            error += fabs(vvv - y[i]);
        }
        //std::cout << j << "th sensor: " << error / (double)rowNum << std::endl;
    }
    
    sw.close();
}


void ST_MVL::sqt2(const arma::mat &x,
                  const arma::vec &y, std::array<double, viewCount + 1> &a,
                  std::array<double, 4> &dt, std::array<double, viewCount> &v)
{
    uint64_t m = x.n_rows; //row count
    uint64_t n = x.n_cols; //column count
    
    uint64_t i, j, k, mm;
    double q, e, u, p, yy, s, r, pp;
    arma::vec b((m + 1) * (m + 1));
    mm = m + 1;
    b[mm * mm - 1] = (double)n;
    for (j = 0; j <= m - 1; j++)
    {
        p = 0.0;
        for (i = 0; i < n; i++)
        {
            p = p + x.at(j, i);
        }
        b[m * mm + j] = p;
        b[j * mm + m] = p;
    }
    for (i = 0; i <= m - 1; i++)
    {
        for (j = i; j <= m - 1; j++)
        {
            p = 0.0;
            for (k = 0; k < n; k++)
            {
                p = p + x.at(i, k) * x.at(j, k);
            }
            b[j * mm + i] = p;
            b[i * mm + j] = p;
        }
    }
    a[m] = 0.0;
    for (i = 0; i < n; i++)
    {
        a[m] = a[m] + y[i];
    }
    for (i = 0; i <= m - 1; i++)
    {
        a[i] = 0.0;
        for (j = 0; j < n; j++)
        {
            a[i] = a[i] + x.at(i, j) * y[j];
        }
    }
    (void)chlk(b, mm, 1, a);
    yy = 0.0;
    for (i = 0; i < n; i++)
    {
        yy = yy + y[i] / (double)n;
    }
    q = 0.0;
    e = 0.0;
    u = 0.0;
    for (i = 0; n != 0 && i < n; i++)
    {
        p = a[m];
        for (j = 0; j <= m - 1; j++)
        {
            p = p + a[j] * x.at(j, i);
        }
        q = q + (y[i] - p) * (y[i] - p);
        e = e + (y[i] - yy) * (y[i] - yy);
        u = u + (yy - p) * (yy - p);
    }
    s = sqrt(q / (double)n);
    r = sqrt(1.0 - q / e);
    for (j = 0; j <= m - 1; j++)
    {
        p = 0.0;
        for (i = 0; i < n; i++)
        {
            pp = a[m];
            for (k = 0; k <= m - 1; k++)
            {
                if (k != j)
                {
                    pp = pp + a[k] * x.at(k, i);
                }
            }
            p = p + (y[i] - pp) * (y[i] - pp);
        }
        v[j] = sqrt(1.0 - q / p);
    }
    dt[0] = q;
    dt[1] = s;
    dt[2] = r;
    dt[3] = u;
}

int ST_MVL::chlk(arma::vec &a, uint64_t n, uint64_t m, std::array<double, viewCount + 1> &d)
{
    uint64_t i, j, k, u, v;
    if ((a[0] + 1.0 == 1.0) || (a[0] < 0.0))
    {
        std::cout << "fail" << std::endl;
        return (-2);
    }
    a[0] = sqrt(a[0]);
    for (j = 1; j < n; j++)
    {
        a[j] = a[j] / a[0];
    }
    for (i = 1; i < n; i++)
    {
        u = i * n + i;
        for (j = 1; j <= i; j++)
        {
            v = (j - 1) * n + i;
            a[u] = a[u] - a[v] * a[v];
        }
        if ((a[u] + 1.0 == 1.0) || (a[u] < 0.0))
        {
            std::cout << "fail" << std::endl;
            return (-2);
        }
        a[u] = sqrt(a[u]);
        if (i != (n - 1))
        {
            for (j = i + 1; j < n; j++)
            {
                v = i * n + j;
                for (k = 1; k <= i; k++)
                {
                    a[v] = a[v] - a[(k - 1) * n + i] * a[(k - 1) * n + j];
                }
                a[v] = a[v] / a[u];
            }
        }
    }
    for (j = 0; j <= m - 1; j++)
    {
        d[j] = d[j] / a[0];
        for (i = 1; i < n; i++)
        {
            u = i * n + i;
            v = i * m + j;
            for (k = 1; k <= i; k++)
            {
                d[v] = d[v] - a[(k - 1) * n + i] * d[(k - 1) * m + j];
            }
            d[v] = d[v] / a[u];
        }
    }
    for (j = 0; j <= m - 1; j++)
    {
        u = (n - 1) * m + j;
        d[u] = d[u] / a[n * n - 1];
        for (k = n - 1; k >= 1; k--)
        {
            u = (k - 1) * m + j;
            for (i = k; i <= n - 1; i++)
            {
                v = (k - 1) * n + i;
                d[u] = d[u] - a[v] * d[i * m + j];
            }
            v = (k - 1) * n + k - 1;
            d[u] = d[u] / a[v];
        }
    }
    return (2);
}

inline bool ST_MVL::ismissing(double val)
{
    //return val == default_missing;
    return std::isnan(val);
}

// Auxiliary

inline double ST_MVL::RadToDeg(double d)
{
    return d * M_PI / 180.0;
}

double ST_MVL::GeoDistance(double lat1, double lng1, double lat2, double lng2)
{
    double radLat1 = RadToDeg(lat1);
    double radLat2 = RadToDeg(lat2);
    double a = radLat1 - radLat2;
    double b = RadToDeg(lng1) - RadToDeg(lng2);
    
    double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
    s = s * EARTH_RADIUS;
    return s;
}

} // namespace Algorithms
