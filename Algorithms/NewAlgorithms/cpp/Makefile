all:
	g++ -O3 -D ARMA_DONT_USE_WRAPPER -o cmake-build-debug/algoCollection -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -fopenmp -std=gnu++14 main.cpp Testing.cpp Performance/Benchmark.cpp MathIO/MatrixReadWrite.cpp Algebra/Auxiliary.cpp Algebra/CentroidDecomposition.cpp Algebra/RSVD.cpp Stats/Correlation.cpp Algorithms/MeanImpute.cpp Algorithms/LinearImpute.cpp -lopenblas -larpack -lmlpack

clean:
	rm cmake-build-debug/algoCollection
