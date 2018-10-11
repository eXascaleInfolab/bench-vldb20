cmake_minimum_required(VERSION 3.9)
project(incCD)

set(CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -pedantic -O3 -msse4.2")

add_executable(incCD
        main.cpp
        CommandLine.h
        Testing.cpp Testing.h

        Performance/Benchmark.cpp Performance/Benchmark.h
        MatrixReadWrite.cpp MatrixReadWrite.h

        LinearAlgebra/Matrix.cpp LinearAlgebra/Matrix.h
        LinearAlgebra/Vector.cpp LinearAlgebra/Vector.h

        LinearAlgebra/CentroidDecomposition.cpp LinearAlgebra/CentroidDecomposition.h
        LinearAlgebra/SVDecomposition.cpp LinearAlgebra/SVDecomposition.h

        LinearAlgebra/MissingBlock.cpp LinearAlgebra/MissingBlock.h
        LinearAlgebra/MissingValueRecovery.cpp LinearAlgebra/MissingValueRecovery.h

        Statistics/CorrelationMatrix.cpp Statistics/CorrelationMatrix.h
        )
