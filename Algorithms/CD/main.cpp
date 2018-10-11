#include <iostream>
#include <tuple>
#include "Testing.h"
#include "MatrixReadWrite.h"
#include "Performance/Benchmark.h"
#include "Statistics/CorrelationMatrix.h"

#include "CommandLine.h"

using namespace Testing;
using namespace std;

int main(int argc, char *argv[])
{
    if (argc == 1) {
        //TestUpdCD();
        //TestIncCD();
        //TestCD();
        //TestMult();
        TestCD_RMV();
        //TestCORR();
        return EXIT_SUCCESS;
    }

    int test_type = -1;
    uint64_t n = 0, m = 0, istep = 1, max = 0, k = 0;
    uint32_t optimization = 0;
    string in = "./data", out = "./out.txt";

    int result = CommandLine(argc, argv, n, m, k, istep, max, optimization, test_type, in, out);

    if (result != 0)
    {
        return result;
    }
    else if (test_type == TEST_REC || test_type == TEST_NORM || test_type == TEST_RECTIME || test_type == TEST_RECTIME_INC || test_type == TEST_REC_INC)
    {
        if (n == 0 || m == 0)
        {
            printUsage();
            return -1;
        }
    }
    else if (n == 0 || m == 0 || max == 0 || istep <= 0 || test_type == -1) // mandatory args not supplied or are garbage
    {
        printUsage();
        return -1;
    }

    LinearAlgebra_Basic::Matrix *mx;
    double *data = MathIO::getMatrixByName(in, n, m);
    mx = new LinearAlgebra_Basic::Matrix(n, m, data);

    if (test_type == TEST_OUTPUT)
    {
        MathIO::closeFile();
        auto output = Benchmark::benchmarkCDoutput(mx);

        LinearAlgebra_Basic::Matrix *Load;
        LinearAlgebra_Basic::Matrix *Rel;
        std::vector<double> centroidValues;

        std::tie(Load, Rel, centroidValues) = output;

        MathIO::writeMatrix(out + ".Load", *Load);
        MathIO::writeMatrix(out + ".Rel", *Rel);
        MathIO::writeVector(out + ".CVal", centroidValues);
    }
    else if (test_type == TEST_SSV)
    {
        vector<uint64_t> *time = Benchmark::benchmarkCDssviter(mx, istep);

        MathIO::closeFile();

        MathIO::writeTime(out, n, m, (*time)[0]);

        for (uint64_t i = 1; i < time->size(); i++)
        {
            MathIO::writeTime(out, ++n, m, (*time)[i]);
        }

        cout << "TIME: " << time << endl;
    }
    else if (test_type == TEST_TIME)
    {
        vector<int64_t> *time = Benchmark::benchmarkCDtime(mx, istep);

        MathIO::closeFile();

        MathIO::writeTime(out, n, m, (*time)[0]);

        for (uint64_t i = 1; i < time->size(); i++)
        {
            MathIO::writeTime(out, ++n, m, (*time)[i]);
        }

        cout << "TIME: " << time << endl;
    }
    else if (test_type == TEST_REC_STREAMING)
    {
        vector<LinearAlgebra_Algorithms::MissingBlock> missingBlocks;
        int64_t res = Benchmark::benchmarkIncCDMVRTime(mx, &missingBlocks, k, istep);

        MathIO::closeFile();

        LinearAlgebra_Basic::Matrix timeResult(1, 1, false);
        timeResult(0,0) = res;

        MathIO::writeMatrix(out, timeResult);
    }
    else if (test_type == TEST_REC)
    {
        vector<LinearAlgebra_Algorithms::MissingBlock> missingBlocks;
        /*int64_t res = */ (void) Benchmark::benchmarkCDMVRTime(mx, &missingBlocks, k, false, optimization);

        MathIO::closeFile();

        //MathIO::writeRecovery(out, n, m, res, &missingBlocks);
        MathIO::writeMatrix(out, *mx);
    }
    else if (test_type == TEST_REC_INC)
    {
        vector<LinearAlgebra_Algorithms::MissingBlock> missingBlocks;
        /*int64_t res = */ (void) Benchmark::benchmarkCDMVRTime(mx, &missingBlocks, k, true, optimization);

        MathIO::closeFile();

        //MathIO::writeRecovery(out, n, m, res, &missingBlocks);
        MathIO::writeMatrix(out, *mx);
    }
    else if (test_type == TEST_RECTIME)
    {
        vector<LinearAlgebra_Algorithms::MissingBlock> missingBlocks;
        int64_t res = Benchmark::benchmarkCDMVRTime(mx, &missingBlocks, k, false, optimization);

        MathIO::closeFile();

        LinearAlgebra_Basic::Matrix timeResult(1, 1, false);
        timeResult(0,0) = res;

        MathIO::writeMatrix(out, timeResult);
    }
    else if (test_type == TEST_RECTIME_INC)
    {
        vector<LinearAlgebra_Algorithms::MissingBlock> missingBlocks;
        int64_t res = Benchmark::benchmarkCDMVRTime(mx, &missingBlocks, k, true, optimization);

        MathIO::closeFile();

        LinearAlgebra_Basic::Matrix timeResult(1, 1, false);
        timeResult(0,0) = res;

        MathIO::writeMatrix(out, timeResult);
    }
    else if (test_type == TEST_NORM)
    {
        Stats::CorrelationMatrix cm(mx);

        cm.normalizeMatrix();

        MathIO::writeMatrix(out, *mx);
    }
    else //if (test_type == TEST_PREC)
    {
        Benchmark::MemoryPrecisionBenchmark *mem = Benchmark::benchmarkCDmemoryprecision(mx);
        MathIO::writeMemory(out, n, m, mem->memory);
        MathIO::writePrecision(out, n, m, mem->precision);
    }

    return 0;
}