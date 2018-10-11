#pragma once

#define mod(x,y) ((((x) % (y)) + (y)) % (y))

typedef struct {
  int k;
  int l;
  int d;
  int L;
  int offset;
  double *ts;
  double **ref_ts;
} Opts;


void TKCM(Opts *opts);
