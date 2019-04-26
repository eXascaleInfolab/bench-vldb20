//
// Created by Zakhar on 07.03.2017.
//

#include <string>
#include <iostream>

#include "Testing.h"
#include "Algebra/CentroidDecomposition.h"
#include "Algorithms/CDMissingValueRecovery.h"
#include "Stats/Correlation.h"
#include "Algebra/Auxiliary.h"
#include "Algebra/RSVD.h"

#include <armadillo>

using namespace Algorithms;
using namespace Stats;

namespace Testing
{

namespace DataSets
{
std::vector<std::vector<double>> synth_missing = {
        {-12.00, 8.00,   -4.00,  -8.00},
        {0.00,   0.00,   0.00,   0.00},
        {-48.00, 32.00,  -16.00, -32.00},
        {NAN,    64.00,  -32.00, -64.00},
        {NAN,    24.00,  -12.00, -24.00},
        {NAN,    64.00,  -32.00, -64.00},
        {NAN,    16.00,  -8.00,  -16.00},
        {NAN,    8.00,   -4.00,  -8.00},
        {NAN,    -32.00, 16.00,  32.00},
        {NAN,    8.00,   -4.00,  -8.00},
        {12.00,  -8.00,  4.00,   8.00},
        {NAN,    -24.00, 12.00,  24.00},
        {NAN,    16.00,  -8.00,  -16.00},
        {-12.00, 8.00,   -4.00,  -8.00},
        {24.00,  -16.00, 8.00,   16.00},
        {NAN,    -8.00,  4.00,   8.00},
        {NAN,    -12.00, 6.00,   12.00},
        {NAN,    -24.00, 12.00,  24.00},
        {48.00,  -32.00, 16.00,  32.00},
        {12.00,  -8.00,  4.00,   8.00}
};

std::vector<std::vector<double>> synth_complete = {
        {-12.00, 8.00,   -4.00,  -8.00},
        {0.00,   0.00,   0.00,   0.00},
        {-48.00, 32.00,  -16.00, -32.00},
        {-96,    64.00,  -32.00, -64.00},
        {-36,    24.00,  -12.00, -24.00},
        {-96,    64.00,  -32.00, -64.00},
        {-24,    16.00,  -8.00,  -16.00},
        {-12,    8.00,   -4.00,  -8.00},
        {48,     -32.00, 16.00,  32.00},
        {-12,    8.00,   -4.00,  -8.00},
        {12.00,  -8.00,  4.00,   8.00},
        {36,     -24.00, 12.00,  24.00},
        {-24,    16.00,  -8.00,  -16.00},
        {-12.00, 8.00,   -4.00,  -8.00},
        {24.00,  -16.00, 8.00,   16.00},
        {12,     -8.00,  4.00,   8.00},
        {18,     -12.00, 6.00,   12.00},
        {36,     -24.00, 12.00,  24.00},
        {48.00,  -32.00, 16.00,  32.00},
        {12.00,  -8.00,  4.00,   8.00}
};

std::vector<std::vector<double>> synth_missing_alt = {
        {NAN,    8.00,   -4.00,  -8.00},
        {NAN,    0.00,   0.00,   0.00},
        {NAN,    32.00,  -16.00, -32.00},
        {-96,    64.00,  -32.00, -64.00},
        {NAN,    24.00,  -12.00, -24.00},
        {-96,   NAN,     -32.00, -64.00},
        {-24,   NAN,     -8.00,  -16.00},
        {-12,    8.00,   -4.00,  -8.00},
        {48,     -32.00, 16.00,  32.00},
        {-12,   NAN,     -4.00,  -8.00},
        {12.00, NAN,     4.00,   8.00},
        {36,    NAN, NAN,       NAN},
        {-24,    16.00,  -8.00, NAN},
        {-12.00, 8.00,   -4.00, NAN},
        {24.00,  -16.00, 8.00,  NAN},
        {12,     -8.00,  4.00,   8.00},
        {18,     -12.00, 6.00,   12.00},
        {36,     -24.00, 12.00,  24.00},
        {48.00, NAN, NAN,        32.00},
        {12.00,  -8.00,  4.00,   8.00}
};
}

//
// Test scenarios
//

void TestCorr()
{
    arma::mat mx = Algebra::Operations::std_to_arma(DataSets::synth_complete);
    Stats::CorrelationMatrix correlationMatrix(mx);
    
    arma::mat &res = correlationMatrix.getCorrelationMatrix();
    res.print("Corr(X) =");
    
    arma::vec sv = correlationMatrix.getSingularValuesOfCM();
    sv.print("Sigma(Corr(X)) =");
}

void TestCD_RMV()
{
    arma::mat mx = Algebra::Operations::std_to_arma(DataSets::synth_missing_alt);
    
    mx.print("X =");
    
    Algorithms::CDMissingValueRecovery mvr(mx, 100, 0.001);
    
    mvr.autoDetectMissingBlocks();
    mvr.setReduction(1);
    
    mvr.performRecovery(true);
    
    mx.print("X_rec =");
}

namespace DataSets
{
std::vector<std::vector<double>> example1 = {
        {-5.63, -1.58, -6.57},
        {-3.37, -0.20, -3.92},
        {-0.82, 4.07,  1.45}
    
};

std::vector<std::vector<double>> example1full = {
        {-5.63, -1.58, -6.57},
        {-3.37, -0.20, -3.92},
        {-0.82, 4.07,  1.45},
        // example1 ^
        {-1.00, -0.02, -4.77},
        {-2.28, -1.60, -3.63},
        {-2.35, 0.20,  -3.18},
        {-1.37, 4.47,  1.68},
        {-5.05, 0.48,  -5.27},
        {-5.43, -1.23, -7.45},
        {-4.82, 0.52,  -4.90},
        {-4.20, 4.77,  -0.55},
        {-5.07, 0.75,  -5.88}
};

std::vector<std::vector<double>> example1full_ext = {
        {-5.63, -1.58, -6.57, 2.98},
        {-3.37, -0.20, -3.92, -4.01},
        {-0.82, 4.07,  1.45,  3.11},
        // example1 ^
        {-1.00, -0.02, -4.77, 1.11},
        {-2.28, -1.60, -3.63, -2.22},
        {-2.35, 0.20,  -3.18, -1.01},
        {-1.37, 4.47,  1.68,  -0.10},
        {-5.05, 0.48,  -5.27, 0.86},
        {-5.43, -1.23, -7.45, 1.08},
        {-4.82, 0.52,  -4.90, 2.45},
        {-4.20, 4.77,  -0.55, -0.51},
        {-5.07, 0.75,  -5.88, -1.41}
};
}

void TestIncCD()
{
    arma::mat mx = Algebra::Operations::std_to_arma(DataSets::example1); // the same one changed by inc. algo
    
    mx.print("X =");
    
    CentroidDecomposition cd(mx);
    
    cd.performDecomposition();
    
    const arma::mat &L = cd.getLoad();
    const arma::mat &R = cd.getRel();
    
    L.print("LOAD =");
    R.print("REL =");
    
    arma::mat reconstructedX = L * R.t();
    
    reconstructedX -= mx;
    
    std::cout << "||X - (L * R^T)||_F = " << arma::norm(reconstructedX, "fro") << std::endl;
    
    for (uint64_t data3iter = 3; data3iter < DataSets::example1full.size(); ++data3iter)
    {
        cd.increment(DataSets::example1full[data3iter]);
        
        cd.performDecomposition();
        
        const arma::mat &L2 = cd.getLoad();
        const arma::mat &R2 = cd.getRel();
        
        reconstructedX = L2 * R2.t();
        
        reconstructedX -= mx;
        
        std::cout << "Incremental iteration #" << data3iter << std::endl;
        std::cout << "||X - (L * R^T)||_F = " << arma::norm(reconstructedX, "fro") << std::endl;
    }
    
    std::cout << "INC:" << std::endl;
    cd.getLoad().print();
    cd.getRel().print();
    
    arma::mat mx3 = Algebra::Operations::std_to_arma(DataSets::example1full);
    Algorithms::CentroidDecomposition cd2(mx3);
    
    cd2.resetSignVectors();
    cd2.performDecomposition();
    
    std::cout << "Batch:" << std::endl;
    cd2.getLoad().print();
    cd2.getRel().print();
    
    arma::mat diff = (cd2.getLoad() - cd.getLoad());
    std::cout << "||L - L_i||_F = " << arma::norm(diff, "fro") << std::endl;
    
    diff = (cd2.getRel() - cd.getRel());
    std::cout << "||R - R_i||_F = " << arma::norm(diff, "fro") << std::endl;
}

void TestCD()
{
    arma::mat mx = Algebra::Operations::std_to_arma(DataSets::example1);
    arma::mat mx2(mx);
    
    mx.print("X =");
    
    CentroidDecomposition cd(mx2);
    
    cd.performDecomposition();
    
    const arma::mat &L = cd.getLoad();
    const arma::mat &R = cd.getRel();
    
    L.print("LOAD =");
    R.print("REL =");
    
    arma::mat reconstructedX = L * R.t();
    reconstructedX.print("L * R^T =");
    
    reconstructedX -= mx;
    std::cout << "||X - (L * R^T)||_F = " << arma::norm(reconstructedX, "fro") << std::endl;
}

namespace DataSets
{
std::vector<std::vector<double>> testdata1 = {
        {2,  -1, 7},
        {8,  6,  -4},
        {-3, -2, 1}
    
};

std::vector<std::vector<double>> testdata2 = {
        {2,  -2},
        {0,  3},
        {-4, 2}
};

std::vector<double> vector1 = {1.4, -2.0, 0.7};
}

void TestBasicOps()
{
    arma::mat m1 = Algebra::Operations::std_to_arma(DataSets::testdata1);
    arma::mat m2 = Algebra::Operations::std_to_arma(DataSets::testdata2);
    
    arma::vec v1 = Algebra::Operations::std_to_arma(DataSets::vector1);
    
    arma::mat m_id = arma::eye<arma::mat>(m2.n_rows, m2.n_rows);
    
    m1.print("M1 =");
    m2.print("M2 =");
    v1.print("V1 =");
    m_id.print("ID =");
    
    // Test 1 - M2 * M1
    
    arma::mat resm = m1 * m2;
    resm.print("M1 * M2 =");
    
    // Test 2 - M2 * ID
    
    resm = m1 * m_id;
    resm.print("M1 * ID =");
    
    // Test 3 - M1 * V1
    
    v1.print("V1 =");
    
    arma::vec resv = m1 * v1;
    resv.print("M1 * V1 =");
    
    // Test 3 - M2 * V1
    
    resv = m2.t() * v1;
    resv.print("M2^T * V1 =");
    
    // Test 4 - scalar & norm of v1
    
    std::cout << "<V1,V1> & ||V1||_2 =" << std::endl << arma::dot(v1, v1)
              << " \t" << arma::norm(v1) << std::endl << std::endl;
    
    // Test 5.1 - normalize and print <,> & norm
    
    v1 /= arma::norm(v1);
    
    v1.print("V1 [normalized] =");
    
    std::cout << "<V1,V1> & ||V1||_2 =" << std::endl << arma::dot(v1, v1)
              << " \t" << arma::norm(v1) << std::endl << std::endl;
}

void TestBasicActions()
{
    arma::mat m1 = Algebra::Operations::std_to_arma(DataSets::testdata1);
    arma::mat m2 = Algebra::Operations::std_to_arma(DataSets::testdata2);
    
    arma::vec v1 = Algebra::Operations::std_to_arma(DataSets::vector1);
    
    arma::mat m_id = arma::eye<arma::mat>(m2.n_rows, m2.n_rows);
    
    m1.print("M1 =");
    m2.print("M2 =");
    v1.print("V1 =");
    m_id.print("ID =");
    
    arma::vec diag = m_id.diag();
    diag.print("diag(M1) =");
    
    arma::mat id_cut = m_id.submat(arma::span(0, 1), arma::span(0, 1));
    id_cut.print("ID(:2, :2) =");
    
    arma::mat m1_cut_fullcol = m1.submat(arma::span(0, 1), arma::span::all);
    m1_cut_fullcol.print("M1(:2, :) =");
    
    arma::mat m1_cut = m1.submat(arma::span(1, 2), arma::span(1, 2));
    m1_cut.print("M1(2:3, 2:3) =");
    
    arma::mat m2_cut = m2.submat(arma::span(1, 2), arma::span(1));
    m2_cut.print("M2(2:3, 2) =");
    
    arma::mat m2_cut_fullcol = m2.submat(arma::span(1), arma::span::all);
    m2_cut_fullcol.print("M2(2, :) =");
    
    arma::vec v1_cut = v1.subvec(arma::span(0, 1));
    v1_cut.print("V1(:2) =");
    
    arma::vec v1_cut_alt = v1.subvec(arma::span(1, 2));
    v1_cut_alt.print("V1(2:3) =");
    
    arma::vec vec_ones = arma::ones<arma::vec>(5);
    vec_ones.print("V_ones =");
}

} //namespace Testing
