//
// Created by Zakhar on 03.04.2017.
//

#pragma once

#include <string>
#include <iostream>

void printUsage()
{
    std::cout << std::endl << "Usage: ./incCD -obligatory arg [-optional arg]" << std::endl
         << std::endl
         << "-test {arg}, -t {arg}" << std::endl
         << "    | arg:" << std::endl
         << "        | out, o      - the result of the recovery" << std::endl
         << "        | runtime, rt - runtime of the recovery" << std::endl
         << "    | choose what to output from the action" << std::endl
         << std::endl
         << "-algorithm {str}, -alg {str}" << std::endl
         << "    | codename of the algorithm to run the recovery on" << std::endl
         << std::endl
         << "-input {str}, -in {str}" << std::endl
         << "    | file name, where to take the input matrix from" << std::endl
         << std::endl
         << "-output {str}, -out {str}" << std::endl
         << "    | file name, where to store the result of <test>" << std::endl
         << std::endl
         << "[-n {int}] default(0)" << std::endl
         << "    | amount of rows to load from the input file" << std::endl
         << "    | 0 - load all of them" << std::endl
         << std::endl
         << "[-m {int}] default(0)" << std::endl
         << "    | amount of columns to load from the input file" << std::endl
         << "    | 0 - load all of them" << std::endl
         << std::endl
         << "[-k {int}] default(m)" << std::endl
         << "    | amount of columns of truncated decomposition to keep" << std::endl
         << "    | 0 (dec) - will be set to be equal to m" << std::endl
         << "    | 0 (rec) - will be automatically detected" << std::endl
         << "[-xtra {string}] default(\"\")" << std::endl
         << "    | extra string to be passed to the algorithm" << std::endl
         << std::endl;
}

enum class PTestType
{
    Output, Runtime, Undefined
};

int CommandLine2(
        int argc, char *argv[],
        PTestType &test, std::string &algoCode,
        std::string &input, std::string &output, std::string &xtra,
        uint64_t &n, uint64_t &m, uint64_t &k
)
{
    std::string temp;
    for (int i = 1; i < argc; ++i)
    {
        temp = argv[i];
        
        // man request
        if (temp.empty())
        {
            continue;
        }
        else if (temp == "--help" || temp == "-help" || temp == "/?")
        {
            printUsage();
            return 1;
        }
            
            // Test type, how the algorithm is applied
        else if (temp == "-test" || temp == "-t")
        {
            ++i;
            temp = argv[i];
            
            if (temp == "out" || temp == "o")
            {
                test = PTestType::Output;
            }
            else if (temp == "runtime" || temp == "rt")
            {
                test = PTestType::Runtime;
            }
            else
            {
                std::cout << "Unrecognized -test argument" << std::endl;
                printUsage();
                return EXIT_FAILURE;
            }
        }
        
        else if (temp == "-algorithm" || temp == "-alg")
        {
            ++i;
            algoCode = argv[i];
        }
            
            // in/out
        else if (temp == "-input" || temp == "-in")
        {
            ++i;
            input = argv[i];
        }
        else if (temp == "-output" || temp == "-out")
        {
            ++i;
            output = argv[i];
        }
            
            // Dimensions to override defaults
        else if (temp == "-n")
        {
            ++i;
            temp = argv[i];
            
            n = static_cast<uint64_t>(stoll(temp));
        }
        else if (temp == "-m")
        {
            ++i;
            temp = argv[i];
            
            m = static_cast<uint64_t>(stoll(temp));
        }
        else if (temp == "-k")
        {
            ++i;
            temp = argv[i];
            
            k = static_cast<uint64_t>(stoll(temp));
        }
        
        else if (temp == "-xtra")
        {
            ++i;
            xtra = argv[i];
        }
        
        else
        {
            std::cout << "Unrecognized CLI parameter" << std::endl;
            printUsage();
            return EXIT_FAILURE;
        }
    }
    
    return EXIT_SUCCESS;
}
