cmake_minimum_required(VERSION 3.9)
project(incCD)

#set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O3 -D ARMA_DONT_USE_WRAPPER -Wall -Werror -Wextra -pedantic -Wconversion -Wsign-conversion -Wimplicit-fallthrough -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -fopenmp")

include(FindLAPACK)
find_package(Armadillo REQUIRED)

add_executable(
        algoCollection

        main.cpp
        MathIO/CommandLine.hpp
        Testing.cpp Testing.h

        Performance/Benchmark.cpp Performance/Benchmark.h
        MathIO/MatrixReadWrite.cpp MathIO/MatrixReadWrite.h

        Algebra/Auxiliary.cpp Algebra/Auxiliary.h

        Algorithms/TKCM.cpp Algorithms/TKCM.h
        Algorithms/ST_MVL.cpp Algorithms/ST_MVL.h
        Algorithms/SPIRIT.cpp Algorithms/SPIRIT.h
        Algorithms/GROUSE.cpp Algorithms/GROUSE.h
        Algorithms/NMFMissingValueRecovery.cpp Algorithms/NMFMissingValueRecovery.h
        Algorithms/DynaMMo.cpp Algorithms/DynaMMo.h
        Algorithms/SVT.cpp Algorithms/SVT.h
        Algorithms/ROSL.cpp Algorithms/ROSL.h
        Algorithms/IterativeSVD.cpp Algorithms/IterativeSVD.h
        Algorithms/SoftImpute.cpp Algorithms/SoftImpute.h

        Algorithms/CDMissingValueRecovery.cpp Algorithms/CDMissingValueRecovery.h
        Algebra/CentroidDecomposition.cpp Algebra/CentroidDecomposition.h
        Algebra/MissingBlock.hpp
        Stats/Correlation.cpp Stats/Correlation.h
        Algebra/RSVD.cpp Algebra/RSVD.h)

target_link_libraries(
        algoCollection
        ${LAPACK_LIBRARIES}
        ${Armadillo_LIBRARIES}
        -lmlpack
)
