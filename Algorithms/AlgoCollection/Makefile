all:
	g++ -O3 -D ARMA_DONT_USE_WRAPPER -o cmake-build-debug/incCD -Wall -Werror -Wextra -pedantic -Wconversion -Wsign-conversion -Wimplicit-fallthrough -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -fopenmp -std=gnu++14 main.cpp Testing.cpp Performance/Benchmark.cpp MathIO/MatrixReadWrite.cpp Algebra/Auxiliary.cpp Algorithms/TKCM.cpp Algorithms/ST_MVL.cpp Algorithms/SPIRIT.cpp Algorithms/GROUSE.cpp Algorithms/NMFMissingValueRecovery.cpp Algorithms/DynaMMo.cpp Algorithms/SVT.cpp Algorithms/ROSL.cpp Algorithms/IterativeSVD.cpp Algorithms/SoftImpute.cpp Algorithms/CDMissingValueRecovery.cpp Algebra/CentroidDecomposition.cpp Stats/Correlation.cpp -lopenblas -lmlpack

clean:
	rm cmake-build-debug/incCD
