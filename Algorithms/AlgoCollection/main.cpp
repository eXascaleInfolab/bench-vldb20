#include <iostream>
#include <tuple>
#include <chrono>

#include "Performance/Benchmark.h"
#include "Testing.h"
#include "MathIO/CommandLine.hpp"
#include "MathIO/MatrixReadWrite.h"

using namespace std;

int main(int argc, char *argv[])
{
    // test suite
    
    if (argc == 1)
    {
        Testing::TestBasicActions();
        cout << endl << "---=========---" << endl << endl;
        Testing::TestBasicOps();
        cout << endl << "---=========---" << endl << endl;
        Testing::TestCD();
        cout << endl << "---=========---" << endl << endl;
        Testing::TestIncCD();
        cout << endl << "---=========---" << endl << endl;
        Testing::TestCorr();
        cout << endl << "---=========---" << endl << endl;
        Testing::TestCD_RMV();
        
        return EXIT_SUCCESS;
    }
    
    // CLI parsing
    
    PTestType test = PTestType::Undefined;
    std::string algoCode;
    std::string input;
    std::string output;
    std::string xtra;
    
    uint64_t n = 0, m = 0, k = 0;
    
    int cliret = CommandLine2(
            argc, argv,
            test, algoCode,
            input, output, xtra,
            n, m, k
    );
    
    #if false
    cout << "Service dump [code=" << cliret << "]" << endl
         << "n = " << n << " m = " << m << " k = " << k << endl
         << "in = " << input << " out = " << output << endl
         << "act = " << (int)action << " test = " << (int)test << endl
         << endl;
    #endif
    
    if (cliret != EXIT_SUCCESS) // parsing failure, command misuse or help request
    {
        return cliret;
    }
    
    // Trivial information
    
    if (test == PTestType::Undefined)
    {
        std::cout << "Test type not specified" << std::endl;
        printUsage();
        return EXIT_FAILURE;
    }
    
    if (input.empty() || output.empty())
    {
        std::cout << "Input or output are not specified" << std::endl;
        printUsage();
        return EXIT_FAILURE;
    }
    
    // now we load the matrix (fixed or not) and determine the remaining parameters
    
    MathIO::MatrixReader reader(input, ' ');
    
    if (!reader.isValid())
    {
        return EXIT_FAILURE;
    }
    
    arma::mat matrix;
    
    if (n > 0 && m > 0)
    {
        matrix = reader.getFixedMatrix(n, m);
    }
    else if (n > 0)
    {
        matrix = reader.getFixedRowMatrix(n);
        m = matrix.n_cols;
    }
    else if (m > 0)
    {
        matrix = reader.getFixedColumnMatrix(m);
    }
    else
    {
        matrix = reader.getFullMatrix();
        m = matrix.n_rows;
    }
    
    // parameters that depend on n, m
    
    if (k > m && algoCode != "st-mvl" && algoCode != "tkcm")
    {
        std::cout << "Truncation factor k can't be larger than m" << std::endl;
        return EXIT_FAILURE;
    }
    
    // set defaults because now we can determine those that depend on n, m
    
    if (k == 0 && algoCode != "st-mvl" && algoCode != "tkcm")
    {
        k = m;
    }
    
    int64_t recov_res;
    
    if (test == PTestType::Runtime)
    {
        recov_res = Performance::Recovery(matrix, k, algoCode, xtra);
        
        MathIO::exportSingleValue(output, recov_res);
    }
    else if (test == PTestType::Output)
    {
        (void) Performance::Recovery(matrix, k, algoCode, xtra);
        
        MathIO::exportMatrix(output, matrix);
    }
    else
    {
        std::cout << "Incorrect or unspecified test type" << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
