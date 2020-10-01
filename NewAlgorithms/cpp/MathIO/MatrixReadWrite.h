//
// Created by Zakhar on 16/03/2017.
//

#pragma once

#include <string>
#include <fstream>

#include <vector>
#include <armadillo>

namespace MathIO
{

class MatrixReader
{
  private:
    std::ifstream file;
    std::string buffer;

    std::vector<double> rowContainer;
    char separator;
    bool fileopen;

  public:
    explicit MatrixReader(std::string &input, char sep);

    MatrixReader(MatrixReader &other) = delete; // disable copying
    MatrixReader(const MatrixReader &other) = delete;

    MatrixReader &operator=(MatrixReader &other) = delete;

    MatrixReader &operator=(const MatrixReader &other) = delete;

    bool isValid();

    arma::mat getFullMatrix();

    arma::mat getFixedMatrix(uint64_t n, uint64_t m);

    arma::mat getFixedRowMatrix(uint64_t n);

    arma::mat getFixedColumnMatrix(uint64_t m);

    bool hasNextLine();

    arma::vec readNextLine();

  private:
    void setFirstRow();

    void setNextRow();
};

void exportAnyPrecision(std::string &output, uint64_t n, uint64_t m, double precision);

void exportAnyRuntime(std::string &output, uint64_t n, uint64_t m, int64_t runtime);

void exportDecompOutput(std::string &output, const arma::mat &Load, const arma::mat &Rel,
                        const std::vector<double> &centroidValues);

void exportMatrix(std::string output, const arma::mat &mx);

/*
    TEMPLATE FUNCTIONS
*/

template<class T>
void exportSingleValue(const std::string &output, T value)
{
    std::ofstream out_file;
    out_file.open(output, std::ios::out);

    out_file << value << std::endl;

    out_file.close();
}

////////////////////

double *getMatrixByName(std::string name, uint64_t n, uint64_t m);

std::vector<double> getNextValue(uint64_t m);

void closeFile();

void writeTime(std::string out, uint64_t n, uint64_t m, int64_t time);

void writeMemory(std::string out, uint64_t n, uint64_t m, uint64_t memory);

void writePrecision(std::string out, uint64_t n, uint64_t m, double precision);

//void writeRecovery(std::string out, uint64_t n, uint64_t m, int64_t result,
//                   std::vector<Algorithms::MissingBlock> *missingBlocks);


/*
    TEMPLATE FUNCTIONS
*/

template<class T>
void writeVector(const std::string &out, std::vector<T> vector)
{
    std::ofstream out_file;
    out_file.open(out, std::ios::out);
    
    for (T elem : vector)
    {
        out_file << elem << std::endl;
    }
    out_file.close();
}

} //namespace MathIO
