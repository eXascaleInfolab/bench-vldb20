//
// Created by Zakhar on 16/03/2017.
//

#include <iostream>
#include <dirent.h>

#include "MatrixReadWrite.h"
#include "../Algebra/MissingBlock.hpp"
#include "../Algebra/Auxiliary.h"

using namespace std;

namespace MathIO
{

//
// Reading
//

MatrixReader::MatrixReader(std::string &input, char sep)
        : file(input),
          separator(sep),
          fileopen(true)
{
    if (!file.is_open())
    {
        std::cout << "Can't open file " << input << std::endl;
        fileopen = false;
    }
}

bool MatrixReader::isValid()
{
    return fileopen;
}

arma::mat MatrixReader::getFullMatrix()
{
    setFirstRow();
    uint64_t m = rowContainer.size();
    arma::mat mat(1, m);
    
    for (uint64_t j = 0; j < m; ++j)
    {
        mat(0, j) = rowContainer[j];
    }
    
    while (hasNextLine())
    {
        setNextRow();
        Algebra::Operations::increment_matrix(mat, rowContainer);
    }
    
    return mat;
}

arma::mat MatrixReader::getFixedMatrix(uint64_t n, uint64_t m)
{
    arma::mat mat(n, m);
    
    rowContainer.reserve(m);
    for (uint64_t j = 0; j < m; ++j)
    {
        rowContainer.push_back(0.0);
    }
    
    for (uint64_t i = 0; i < n; ++i)
    {
        setNextRow();
        for (uint64_t j = 0; j < m; ++j)
        {
            mat(i, j) = rowContainer[j];
        }
    }
    
    return mat;
}

arma::mat MatrixReader::getFixedRowMatrix(uint64_t n)
{
    setFirstRow();
    uint64_t m = rowContainer.size();
    
    arma::mat mat(n, m);
    
    for (uint64_t j = 0; j < m; ++j)
    {
        mat(0, j) = rowContainer[j];
    }
    
    for (uint64_t i = 1; i < n; ++i)
    {
        setNextRow();
        for (uint64_t j = 0; j < m; ++j)
        {
            mat(i, j) = rowContainer[j];
        }
    }
    
    return mat;
}

arma::mat MatrixReader::getFixedColumnMatrix(uint64_t m)
{
    arma::mat mat(1, m);
    
    rowContainer.reserve(m);
    for (uint64_t j = 0; j < m; ++j)
    {
        rowContainer.push_back(0.0);
    }
    
    setNextRow(); // in fact first, but we know m, so it's a different call
    for (uint64_t j = 0; j < m; ++j)
    {
        mat(0, j) = rowContainer[j];
    }
    
    while (hasNextLine())
    {
        setNextRow();
        Algebra::Operations::increment_matrix(mat, rowContainer);
    }
    
    return mat;
}

bool MatrixReader::hasNextLine()
{
    return file.peek() != EOF;
}

arma::vec MatrixReader::readNextLine()
{
    setNextRow();
    
    return arma::vec(rowContainer);
}

void MatrixReader::setFirstRow()
{
    getline(file, buffer);
    size_t pos = 0;
    
    while (pos < buffer.size())
    {
        size_t newpos;
        newpos = buffer.find(separator, pos + 1);
        
        newpos = newpos == string::npos ? buffer.length() : newpos;
        
        std::string temp = buffer.substr(pos, newpos - pos);
        
        if (temp.empty())
        { break; }
        
        rowContainer.push_back(stod(temp));
        
        pos = newpos + 1;
    }
}

void MatrixReader::setNextRow()
{
    uint64_t m = rowContainer.size();
    
    getline(file, buffer);
    size_t pos = 0;
    
    for (uint64_t j = 0; j < m; ++j)
    {
        size_t newpos;
        newpos = buffer.find(separator, pos + 1);
        
        newpos = newpos == string::npos ? buffer.length() : newpos;
        
        std::string temp = buffer.substr(pos, newpos - pos);
        
        rowContainer[j] = stod(temp);
        
        pos = newpos + 1;
    }
}

void exportAnyPrecision(std::string &output, uint64_t n, uint64_t m, double precision)
{
    ofstream out_file;
    out_file.open(output, ios::out | ios::app);
    
    out_file << n << "\t" << m << "\t" << precision << endl;
    
    out_file.close();
}

void exportAnyRuntime(std::string &output, uint64_t n, uint64_t m, int64_t runtime)
{
    ofstream out_file;
    out_file.open(output, ios::out | ios::app);
    
    out_file << n << "\t" << m << "\t" << runtime << endl;
    
    out_file.close();
}

void exportDecompOutput(std::string &output, const arma::mat &Load, const arma::mat &Rel,
                        const std::vector<double> &centroidValues)
{
    exportMatrix(output + ".Load", Load);
    
    exportMatrix(output + ".Rel", Rel);
    
    ofstream out_file;
    // Centroid Values
    out_file.open(output + ".Centroid", ios::out);
    for (double elem : centroidValues)
    {
        out_file << elem << std::endl;
    }
    out_file.close();
}

void exportMatrix(std::string output, const arma::mat &mx)
{
    ofstream out_file;
    out_file.open(output, ios::out);
    
    for (uint64_t i = 0; i < mx.n_rows; ++i)
    {
        for (uint64_t j = 0; j < mx.n_cols - 1; ++j)
        {
            out_file << mx(i, j) << " ";
        }
        out_file << mx(i, mx.n_cols - 1) << std::endl;
    }
    out_file.close();
}

} // namespace MathIO
