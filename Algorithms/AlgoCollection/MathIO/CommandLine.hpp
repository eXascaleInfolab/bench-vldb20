//
// Created by Zakhar on 03.04.2017.
//

#pragma once

#include <string>
#include <iostream>

using namespace std;

void printUsage()
{
    cout << endl << "Usage: ./incCD -obligatory arg [-optional arg]" << endl
         << endl
         << "-test {arg}, -t {arg}" << endl
         << "    | arg:" << endl
         << "        | out, o      - the result of the recovery" << endl
         << "        | runtime, rt - runtime of the recovery" << endl
         << "    | choose what to output from the action" << endl
         << endl
         << "-algorithm {str}, -alg {str}" << endl
         << "    | codename of the algorithm to run the recovery on" << endl
         << endl
         << "-input {str}, -in {str}" << endl
         << "    | file name, where to take the input matrix from" << endl
         << endl
         << "-output {str}, -out {str}" << endl
         << "    | file name, where to store the result of <test>" << endl
         << endl
         << "[-n {int}] default(0)" << endl
         << "    | amount of rows to load from the input file" << endl
         << "    | 0 - load all of them" << endl
         << endl
         << "[-m {int}] default(0)" << endl
         << "    | amount of columns to load from the input file" << endl
         << "    | 0 - load all of them" << endl
         << endl
         << "[-k {int}] default(m)" << endl
         << "    | amount of columns of truncated decomposition to keep" << endl
         << "    | 0 (dec) - will be set to be equal to m" << endl
         << "    | 0 (rec) - will be automatically detected" << endl
         << "[-xtra {string}] default(\"\")" << endl
         << "    | extra string to be passed to the algorithm" << endl
         << endl;
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
    string temp;
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
                cout << "Unrecognized -test argument" << endl;
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
            cout << "Unrecognized CLI parameter" << endl;
            printUsage();
            return EXIT_FAILURE;
        }
    }
    
    return EXIT_SUCCESS;
}
