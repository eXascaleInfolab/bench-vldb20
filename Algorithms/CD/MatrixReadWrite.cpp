//
// Created by Zakhar on 16/03/2017.
//

#include <iostream>
#include <fstream>
#include <dirent.h>
#include "MatrixReadWrite.h"

using namespace std;

namespace MathIO {

//
// Reading
//

// global vars
ifstream file;
string buffer;

double *getMatrixByName(string name, uint64_t n, uint64_t m)
{
    file = ifstream( name );

    if (!file.is_open())
    {
        std::cout << "Can't open file " << name << std::endl;
        return nullptr;
    }

    double *data = static_cast<double *>(malloc(n * m * sizeof(*data)));

    uint64_t i = 0;

    while (!file.eof() && i < n)
    {
        getline(file, buffer);

        size_t pos = 0;

        string temp;
        for (uint64_t j = 0; j < m; ++j)
        {
            // TBA
            size_t newpos;
            newpos = buffer.find(' ', pos + 1);

            newpos = newpos == string::npos ? buffer.length() : newpos;

            temp = buffer.substr(pos, newpos - pos);

            *(data + i * m + j) = stod(temp);

            pos = newpos + 1;
        }

        ++i;
    }

    return data;
}

vector<double> *getNextValue(uint64_t m)
{
    vector<double> *newDataInput = new vector<double>(m);

    if (!file.eof())
    {
        getline(file, buffer);

        size_t pos = 0;

        string temp;
        for (uint64_t j = 0; j < m; ++j)
        {
            // TBA
            size_t newpos;
            newpos = buffer.find(' ', pos + 1);

            newpos = newpos == string::npos ? buffer.length() : newpos;

            temp = buffer.substr(pos, newpos - pos);

            (*newDataInput)[j] = stod(temp);

            pos = newpos + 1;
        }
    }
    else
    {
        delete newDataInput;
        return nullptr;
    }

    return newDataInput;
}

void closeFile()
{
    file.close();
}

//TBA

void writeTime(std::string out, uint64_t n, uint64_t m, int64_t time)
{
    ofstream out_file;
    out_file.open (out, ios::out | ios::app);

    out_file << n << "\t" << m << "\t" << time << endl;

    out_file.close();
}

void writeMemory(std::string out, uint64_t n, uint64_t m, uint64_t memory)
{
    ofstream out_file;
    out_file.open (out, ios::out | ios::app);

    out_file << n << "\t" << m << "\t" << memory << endl;

    out_file.close();
}

void writePrecision(std::string out, uint64_t n, uint64_t m, double precision)
{
    ofstream out_file;
    out_file.open (out, ios::out | ios::app);

    out_file << "PREC: " << n << "\t" << m << "\t" << precision << endl;

    out_file.close();
}

void writeRecovery(std::string out, uint64_t n, uint64_t m, int64_t result,
                   std::vector<LinearAlgebra_Algorithms::MissingBlock> *missingBlocks)
{
    ofstream out_file;
    out_file.open (out, ios::out | ios::app);

    out_file << "Recovery in " << n << " x " << m << " matrix performed in : " << result << "\t" << "ms" << endl
            << "Recovered blocks:" << endl;

    for (auto mblock : *missingBlocks)
    {
        auto v = LinearAlgebra_Basic::Vector(mblock.blockSize, mblock.extractBlock(), true);
        out_file << "MB_" << mblock.column << "," << mblock.startingIndex << "," << mblock.blockSize << " =" << v.toString() << std::endl;
    }

    out_file.close();
}

void writeMatrix(std::string out, LinearAlgebra_Basic::Matrix &mx)
{
    ofstream out_file;
    out_file.open (out, ios::out);

    //out_file << std::fixed << std::setprecision(2);

    for (uint64_t i = 0; i < mx.dimensionN(); ++i)
    {
        for (uint64_t j = 0; j < mx.dimensionM() - 1; ++j)
        {
            out_file << mx(i, j) << " ";
        }
        out_file << mx(i, mx.dimensionM() - 1) << std::endl;
    }
    //out_file << mx.toString();
    out_file.close();
}

template void writeVector<double>(const std::string &out, std::vector<double> vector);

template<class T>
void writeVector(const std::string &out, std::vector<T> vector)
{
    ofstream out_file;
    out_file.open (out, ios::out);

    for (T elem : vector)
    {
        out_file << elem << std::endl;
    }
    out_file.close();
}


} // namespace MathIO