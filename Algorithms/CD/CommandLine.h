//
// Created by Zakhar on 03.04.2017.
//

#include <cstdint>
#include <string>
#include <iostream>

#define TEST_PREC 0
#define TEST_TIME 1
#define TEST_SSV 2
#define TEST_REC 3
#define TEST_NORM 4
#define TEST_RECTIME 5
#define TEST_RECTIME_INC 8
#define TEST_REC_INC 6
#define TEST_REC_STREAMING 7
#define TEST_OUTPUT 9

using namespace std;

void printUsage()
{
    cout << "Usage: centroiddecomposition [args]" << endl
         << "-test o  : output test (CD)" << endl
         << "-test p  : precision test (CD)" << endl
         << "-test t  : time test (CD)" << endl
         << "-test r  : recovery test (CD based)" << endl
         << "-test rc : recovery test (cached CD based)" << endl
         << "-test rt : time recovery test (CD based)" << endl
         << "-test rtc: time recovery test (cached CD based)" << endl
         << "-test rs : time recovery test (CD based, streaming)" << endl
         << "-test n  : only normalize matrix" << endl
         << "-n #     : # of initial rows value" << endl
         << "-m #     : # of column value" << endl
         << "-k #     : truncation dimension" << endl
         << "-istep # : [default=1] # of rows to increment every iteration" << endl
         << "-max #   : # of maximum rows value (-1 to fetch as long as possible, 1 to do batch only)" << endl
         << "-opt #   : type of runtime optimization to apply to CD based recovery" << endl
         << "-in F    : [default=./data] read matrix from file F" << endl
         << "-out F   : [default=./out.txt] write to file F" << endl;
}

int CommandLine(int argc, char *argv[],
                uint64_t &n, uint64_t &m, uint64_t &k,
                uint64_t &istep, uint64_t &max, uint32_t &optimizaiton,
                int &test_type, string &in, string &out)
{
    string temp;
    for (int i = 1; i < argc; ++i)
    {
        temp = argv[i];

        if (!temp.compare("-test"))
        {
            ++i;
            temp = argv[i];

            if (!temp.compare("o"))
            {
                test_type = TEST_OUTPUT;
            }
            else if (!temp.compare("t"))
            {
                test_type = TEST_TIME;
            }
            else if (!temp.compare("m"))
            {
                test_type = TEST_PREC;
            }
            else if (!temp.compare("s"))
            {
                test_type = TEST_SSV;
            }
            else if (!temp.compare("r"))
            {
                test_type = TEST_REC;
            }
            else if (!temp.compare("rc"))
            {
                test_type = TEST_REC_INC;
            }
            else if (!temp.compare("n"))
            {
                test_type = TEST_NORM;
            }
            else if (!temp.compare("rt"))
            {
                test_type = TEST_RECTIME;
            }
            else if (!temp.compare("rtc"))
            {
                test_type = TEST_RECTIME_INC;
            }
            else if (!temp.compare("rs"))
            {
                test_type = TEST_REC_STREAMING;
            }
            else
            {
                cout << "Invalid -test argument, testing type not recognized" << endl;
                printUsage();
                return -1;
            }
        }
        else if (!temp.compare("-n"))
        {
            ++i;
            temp = argv[i];
            n = static_cast<uint64_t>(stoll(temp));
        }
        else if (!temp.compare("-m"))
        {
            ++i;
            temp = argv[i];
            m = static_cast<uint64_t>(stoll(temp));
        }
        else if (!temp.compare("-k"))
        {
            ++i;
            temp = argv[i];
            k = static_cast<uint64_t>(stoll(temp));
        }
        else if (!temp.compare("-istep"))
        {
            ++i;
            temp = argv[i];
            istep = static_cast<uint64_t>(stoll(temp));
        }
        else if (!temp.compare("-max"))
        {
            ++i;
            temp = argv[i];
            max = static_cast<uint64_t>(stoll(temp)); // should get uint64_max with -1 as input
        }
        else if (!temp.compare("-opt"))
        {
            ++i;
            temp = argv[i];
            optimizaiton = static_cast<uint32_t>(stoll(temp));
        }
        else if (!temp.compare("-in"))
        {
            ++i;
            in = argv[i];
        }
        else if (!temp.compare("-out"))
        {
            ++i;
            out = argv[i];
        }
        else
        {
            cout << "Unrecognized CLI argument" << endl;
            printUsage();
            return -1;
        }
    }

    return 0;
}