//
// Created by Zakhar on 16/03/2017.
//

#ifndef CENTROIDDECOMPOSITION_BENCHMARK_H
#define CENTROIDDECOMPOSITION_BENCHMARK_H

#include <cstdint>
#include "../LinearAlgebra/Matrix.h"
#include "../LinearAlgebra/MissingBlock.h"

namespace Benchmark {

class MemoryPrecisionBenchmark {
  public:
    double precision;
    uint64_t memory;
};

std::vector<int64_t> *benchmarkCDtime(LinearAlgebra_Basic::Matrix *mx, uint64_t istep);

std::tuple<LinearAlgebra_Basic::Matrix *, LinearAlgebra_Basic::Matrix *, std::vector<double>> benchmarkCDoutput(LinearAlgebra_Basic::Matrix *mx);

std::vector<uint64_t> *benchmarkCDssviter(LinearAlgebra_Basic::Matrix *mx, uint64_t istep);

int64_t benchmarkCDMVRTime(LinearAlgebra_Basic::Matrix *mx, std::vector<LinearAlgebra_Algorithms::MissingBlock> *missingblocks,
                           uint64_t k, bool useCache, uint32_t optimization);

int64_t benchmarkIncCDMVRTime(LinearAlgebra_Basic::Matrix *mx, std::vector<LinearAlgebra_Algorithms::MissingBlock> *missingblocks,
                              uint64_t k, uint64_t istep);

MemoryPrecisionBenchmark *benchmarkCDmemoryprecision(LinearAlgebra_Basic::Matrix *mx);

} // namespace Benchmark

#endif //CENTROIDDECOMPOSITION_BENCHMARK_H
