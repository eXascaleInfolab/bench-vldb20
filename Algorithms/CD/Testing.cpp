//
// Created by Zakhar on 07.03.2017.
//

#include <string>
#include <iostream>
#include <cmath>
#include "Testing.h"
#include "LinearAlgebra/Matrix.h"
#include "LinearAlgebra/CentroidDecomposition.h"
#include "LinearAlgebra/MissingValueRecovery.h"
#include "Statistics/CorrelationMatrix.h"

using namespace LinearAlgebra_Basic;
using namespace LinearAlgebra_Algorithms;
using namespace Stats;

namespace Testing {

//
// Data sets
//

std::vector<std::vector<double>> data1 = {
        /*
        { 2, -1, 7 },
        { 8, 6, -4 },
        { -3, -2, 1 }
        */
        { -5.63, -1.58, -6.57 },
        { -3.37, -0.20, -3.92 },
        { -0.82,  4.07,  1.45 }

};

std::vector<std::vector<double>> data2 = {
        { 6, 1.5, -1 },
        { 2.3, 1, 0 }
        /*
        { 2, -2 },
        { 0, 3 },
        { -4,  2 }
         */
};

std::vector<double> vector1 =
        { 1.4, -2.0, 0.0 };

std::vector<std::vector<double>> data3 = {
        { -5.63, -1.58, -6.57 },
        { -3.37, -0.20, -3.92 },
        { -0.82,  4.07,  1.45 },
        // data1 ^
        { -1.00, -0.02, -4.77 },
        { -2.28, -1.60, -3.63 },
        { -2.35,  0.20, -3.18 },
        { -1.37,  4.47,  1.68 },
        { -5.05,  0.48, -5.27 },
        { -5.43, -1.23, -7.45 },
        { -4.82,  0.52, -4.90 },
        { -4.20,  4.77, -0.55 },
        { -5.07,  0.75, -5.88 }
};

std::vector<std::vector<double>> data4 = {
        { -5.63, -1.58, -6.57,  2.98 },
        { -3.37, -0.20, -3.92, -4.01 },
        { -0.82,  4.07,  1.45,  3.11 },
        // data1 ^
        { -1.00, -0.02, -4.77,  1.11 },
        { -2.28, -1.60, -3.63, -2.22 },
        { -2.35,  0.20, -3.18, -1.01 },
        { -1.37,  4.47,  1.68, -0.10},
        { -5.05,  0.48, -5.27,  0.86 },
        { -5.43, -1.23, -7.45,  1.08 },
        { -4.82,  0.52, -4.90,  2.45 },
        { -4.20,  4.77, -0.55, -0.51 },
        { -5.07,  0.75, -5.88, -1.41 }
};

std::vector<std::vector<double>> data5 = {
        {-12.00,	8.00,	-4.00,	-8.00},
        {0.00,	0.00,	0.00,	0.00},
        {-48.00,	32.00,	-16.00,	-32.00},
        {NAN,	64.00,	-32.00,	-64.00},
        {NAN,	24.00,	-12.00,	-24.00},
        {NAN,	64.00,	-32.00,	-64.00},
        {NAN,	16.00,	-8.00,	-16.00},
        {NAN,	8.00,	-4.00,	-8.00},
        {NAN,	-32.00,	16.00,	32.00},
        {NAN,	8.00,	-4.00,	-8.00},
        {12.00,	-8.00,	4.00,	8.00},
        {NAN,	-24.00,	12.00,	24.00},
        {NAN,	16.00,	-8.00,	-16.00},
        {-12.00,	8.00,	-4.00,	-8.00},
        {24.00,	-16.00,	8.00,	16.00},
        {NAN,	-8.00,	4.00,	8.00},
        {NAN,	-12.00,	6.00,	12.00},
        {NAN,	-24.00,	12.00,	24.00},
        {48.00,	-32.00,	16.00,	32.00},
        {12.00,	-8.00,	4.00,	8.00}
};

std::vector<std::vector<double>> data6 = {
        {-12.00,	8.00,	-4.00,	-8.00},
        {0.00,	0.00,	0.00,	0.00},
        {-48.00,	32.00,	-16.00,	-32.00},
        {-96,	64.00,	-32.00,	-64.00},
        {-36,	24.00,	-12.00,	-24.00},
        {-96,	64.00,	-32.00,	-64.00},
        {-24,	16.00,	-8.00,	-16.00},
        {-12,	8.00,	-4.00,	-8.00},
        {48,	-32.00,	16.00,	32.00},
        {-12,	8.00,	-4.00,	-8.00},
        {12.00,	-8.00,	4.00,	8.00},
        {36,	-24.00,	12.00,	24.00},
        {-24,	16.00,	-8.00,	-16.00},
        {-12.00,	8.00,	-4.00,	-8.00},
        {24.00,	-16.00,	8.00,	16.00},
        {12,	-8.00,	4.00,	8.00},
        {18,	-12.00,	6.00,	12.00},
        {36,	-24.00,	12.00,	24.00},
        {48.00,	-32.00,	16.00,	32.00},
        {12.00,	-8.00,	4.00,	8.00}
};

std::vector<std::vector<double>> data7 = {
        {NAN,	8.00,	-4.00,	-8.00},
        {NAN,	0.00,	0.00,	0.00},
        {NAN,	32.00,	-16.00,	-32.00},
        {-96,	64.00,	-32.00,	-64.00},
        {NAN,	24.00,	-12.00,	-24.00},
        {-96,	NAN,	-32.00,	-64.00},
        {-24,	NAN,	-8.00,	-16.00},
        {-12,	8.00,	-4.00,	-8.00},
        {48,	-32.00,	16.00,	32.00},
        {-12,	NAN,	-4.00,	-8.00},
        {12.00,	NAN,	4.00,	8.00},
        {36,	NAN,	NAN,	NAN},
        {-24,	16.00,	-8.00,	NAN},
        {-12.00,	8.00,	-4.00,	NAN},
        {24.00,	-16.00,	8.00,	NAN},
        {12,	-8.00,	4.00,	8.00},
        {18,	-12.00,	6.00,	12.00},
        {36,	-24.00,	12.00,	24.00},
        {48.00,	NAN,	NAN,	32.00},
        {12.00,	-8.00,	4.00,	8.00}
};

std::vector<std::vector<double>> data8 = {
        {6,	8.00},
        {-2,	0.00},
        {NAN,	32.00},
        {NAN,	64.00},
        {2,	24.00}
};
//
// Test scenarios
//

void TestCORR()
{
    Matrix *mx = new Matrix(Testing::data6);
    Stats::CorrelationMatrix corellationMatrix(mx);

    Matrix *res = corellationMatrix.getCorrelationMatrix();

    std::cout << "Corr(X) =" << std::endl << res->toString() << std::endl;

    Vector *sv = corellationMatrix.getSingularValuesOfCM();

    std::cout << "Sigma(Corr(X)) =" << std::endl << sv->toString() << std::endl;
}

void TestCD_RMV()
{
    Matrix *mx = new Matrix(Testing::data8);

    std::cout << "X =" << std::endl << mx->toString() << std::endl;

    MissingValueRecovery mvr = MissingValueRecovery(mx, 100, 0.001); //34, 45

    mvr.autoDetectMissingBlocks();

    //mvr.addMissingBlock(0, 3, 7);  // MB_reference = (-96.00; -36.00; -96.00; -24.00; -12.00; 48.00; -12.00;)^T
    //mvr.addMissingBlock(0, 11, 2); // MB_reference = (36.00; -24.00)^T
    //mvr.addMissingBlock(0, 15, 3); // MB_reference = (12.00; 18.00; 36.00)^T

    mvr.setReduction(1);

    mvr.performRecovery(true);

    std::cout << "X_rec =" << std::endl << mx->toString() << std::endl;
}

void TestIncCD()
{
    Matrix *mx = new Matrix(Testing::data1); // the same one changed by inc. algo

    std::cout << "X =" << std::endl << mx->toString() << std::endl;

    CentroidDecomposition cd = CentroidDecomposition(mx);

    cd.performDecomposition(true);

    Matrix &L = *cd.getLoad();
    Matrix &R = *cd.getRel();

    std::cout << "LOAD =" << std::endl << L.toString() << std::endl;

    std::cout << "REL =" << std::endl << R.toString() << std::endl;

    Matrix *reconstructedX = matrix_mult_A_BT(L, R);

    *reconstructedX -= *mx;

    std::cout << "||X - (L * R^T)||_F = " << reconstructedX->normF() << std::endl;

    delete reconstructedX;

    for (uint64_t data3iter = 3; data3iter < data3.size(); ++data3iter)
    {
        cd.increment(data3[data3iter]);

        cd.performDecomposition(true);

        Matrix &L2 = *cd.getLoad();
        Matrix &R2 = *cd.getRel();

        reconstructedX = matrix_mult_A_BT(L2, R2);

        *reconstructedX -= *mx;

        std::cout << "Incremental iteration #" << data3iter << std::endl;
        std::cout << "||X - (L * R^T)||_F = " << reconstructedX->normF() << std::endl;

        delete reconstructedX;
    }

    std::cout << "INC:" << std::endl;
    std::cout << cd.getLoad()->toString() << std::endl;
    std::cout << cd.getRel()->toString() << std::endl;

    Matrix *mx3 = new Matrix(data3);
    CentroidDecomposition cd2 = CentroidDecomposition(mx3);

    cd2.performDecomposition(false);

    std::cout << "Batch:" << std::endl;
    std::cout << cd2.getLoad()->toString() << std::endl;
    std::cout << cd2.getRel()->toString() << std::endl;

    Matrix *diff;
    diff = (*cd2.getLoad() - *cd.getLoad());
    std::cout << "||L - L_i||_F = " << diff->normF() << std::endl;
    delete diff;

    diff = (*cd2.getRel() - *cd.getRel());
    std::cout << "||R - R_i||_F = " << diff->normF() << std::endl;
    delete diff;
    delete mx;
    delete mx3;
}

void TestCD()
{
    Matrix *mx = new Matrix(Testing::data1);
    Matrix *mx2 = mx->copy();

    std::cout << "X =" << std::endl << mx->toString() << std::endl;

    CentroidDecomposition cd = CentroidDecomposition(mx2);

    cd.performDecomposition(false);

    Matrix &L = *cd.getLoad();
    Matrix &R = *cd.getRel();

    std::cout << "LOAD =" << std::endl << L.toString() << std::endl;

    std::cout << "REL =" << std::endl << R.toString() << std::endl;

    Matrix *reconstructedX = matrix_mult_A_BT(L, R);

    std::cout << "L * R^T: " << std::endl << reconstructedX->toString() << std::endl;

    *reconstructedX -= *mx;

    std::cout << "||X - (L * R^T)||_F = " << reconstructedX->normF() << std::endl;

    delete reconstructedX;
    delete mx;
    delete mx2;
}

void TestMult()
{
    Matrix *m1 = new Matrix(Testing::data1);
    Matrix *m2 = new Matrix(Testing::data2);
    Vector *v1 = new Vector(Testing::vector1);

    Matrix *resm;
    Vector *resv;

    Matrix *m_id = Matrix::identity(m2->dimensionM());

    // Test 1 - M2 * M1
    std::cout << "M1:" << std::endl << m1->toString() << std::endl;
    std::cout << "M2:" << std::endl << m2->toString() << std::endl;

    resm = (*m2) * (*m1);

    std::cout << "M2 * M1: " << std::endl << resm->toString() << std::endl;

    delete resm;

    // Test 2 - M2 * ID

    std::cout << "ID: " << std::endl << m_id->toString() << std::endl;

    resm = (*m2) * (*m_id);

    std::cout << "M2 * ID: " << std::endl << resm->toString() << std::endl;

    delete resm;

    // Test 3 - M1 * V1

    std::cout << "V1: " << std::endl << v1->toString() << std::endl;

    resv = (*m1) * (*v1);

    std::cout << "M1 * V1: " << std::endl << resv->toString() << std::endl;

    delete resv;

    // Test 3 - M2 * V1

    resv = (*m2) * (*v1);

    std::cout << "M2 * V1: " << std::endl << resv->toString() << std::endl;

    delete resv;

    // Test 4 - scalar & norm of v1

    std::cout << "<V1,V1> & ||V1||_2: " << std::endl << vector_dot(*v1, *v1)
            << " \t" << v1->norm2() << std::endl << std::endl;

    // Test 5.1 - normalize and print <,> & norm

    (void)v1->normalize();

    std::cout << "V1 [normalized]: " << std::endl << v1->toString() << std::endl;

    std::cout << "<V1,V1> & ||V1||_2: " << std::endl << vector_dot(*v1, *v1)
              << " \t" << v1->norm2() << std::endl << std::endl;

    // cleanup

    delete m1;
    delete m2;
    delete v1;
    delete m_id;
}

} //namespace Testing