//
// Created by Zakhar on 08.03.2017.
//

#include "Matrix.h"

#ifndef CENTROIDDECOMPOSITION_CENTROIDDECOMPOSITION_H
#define CENTROIDDECOMPOSITION_CENTROIDDECOMPOSITION_H

namespace LinearAlgebra_Algorithms {

class CentroidDecomposition {
    //
    // Data
    //
  private:
    LinearAlgebra_Basic::Matrix *Src;
    LinearAlgebra_Basic::Matrix *Load = nullptr;
    LinearAlgebra_Basic::Matrix *Rel = nullptr;

  public:
    std::vector<LinearAlgebra_Basic::Vector *> signVector;
    std::vector<std::vector<LinearAlgebra_Basic::Vector *>> signVectorSteps;
    uint64_t ssvIterations = 0;
    uint64_t truncation = 0;

    //
    // Constructors & desctructors
    //
  public:
    explicit CentroidDecomposition(LinearAlgebra_Basic::Matrix *mx);
    CentroidDecomposition(LinearAlgebra_Basic::Matrix *mx, uint64_t k);
    ~CentroidDecomposition();

    //
    // API
    //
  public:
    LinearAlgebra_Basic::Matrix *getLoad();
    LinearAlgebra_Basic::Matrix *getRel();

    void performDecomposition(bool incremental, std::vector<double> *centroidValues = nullptr, bool stopOnIncompleteRank = false, bool skipSSV = false);
    void increment(LinearAlgebra_Basic::Vector *vec);
    void increment(std::vector<double> &vec);

    //
    // Algorithm
    //
  private:
    bool decomposed = false;
    uint64_t addedRows = 0;
    LinearAlgebra_Basic::Vector *findIncrementalSSV(LinearAlgebra_Basic::Matrix *mx, uint64_t k);
    LinearAlgebra_Basic::Vector *findIncrementalSSVPlus(LinearAlgebra_Basic::Matrix *mx, uint64_t k);

  public:
    static constexpr double eps = 1E-11;
};

} // namespace LinearAlgebra_Algorithms

#endif //CENTROIDDECOMPOSITION_CENTROIDDECOMPOSITION_H
