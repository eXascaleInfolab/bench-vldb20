#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tkcm.h"

#define POS(row,col) (((nr_patterns+1)*(row)) + (col))

void TKCM(Opts *opts)
{
  int nr_patterns = opts->L - 2*opts->l + 1;
  double *M = calloc((opts->k+1)*(nr_patterns+1), sizeof(double));
  double *D = calloc(nr_patterns+1, sizeof(double));
  int *A = calloc(opts->k, sizeof(int));

  // step 1: compute pattern dissimilarities
  for (int j = 1; j <= opts->L-2*opts->l+1; ++j) {
    D[j] = 0;
    for (int i = 0; i < opts->d; ++i) {
      for (int x = 0; x <= opts->l-1; x++) {
        int pos = opts->offset + opts->l + j - 1 - x;
        double x1 = opts->ref_ts[i][mod(pos, opts->L)];
        double x2 = opts->ref_ts[i][mod(opts->offset - x, opts->L)];
        D[j] += pow(x1-x2, 2);
      }
    }
    D[j] = sqrt(D[j]);
  }

  // step 2.1: dynamic programming
  for (int j = 0; j <= opts->L-2*opts->l+1; ++j) {
    M[POS(0,j)] = 0;
    for (int i = 1; i <= opts->k; ++i) {
      if (i > j) {
        M[POS(i,j)] = INFINITY;
      } else {
        int pred = j-opts->l >= 0 ? j-opts->l : 0;
        M[POS(i,j)] = fmin(M[POS(i,j-1)], D[j]+M[POS(i-1,pred)]);
      }
    }
  }

  // step 2.2: backtracking
  int i = opts->k;
  int j = opts->L - 2*opts->l + 1;
  while (i > 0) {
    if (M[POS(i,j)] == M[POS(i,j-1)]) {
      --j;
    } else {
      A[i-1] = j;
      --i;
      j = j-opts->l >= 0 ? j-opts->l : 0;
    }
  }

  // step 3: impute missing value
  double sum = 0;
  for (int i = 0; i < opts->k; ++i) {
    int pos = opts->offset + opts->l + A[i] - 1;
    sum += opts->ts[mod(pos, opts->L)];
  }
  opts->ts[opts->offset] = sum / opts->k;

  free(M);
  free(D);
  free(A);
}
