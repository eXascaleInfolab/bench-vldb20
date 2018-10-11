#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include "tkcm.h"
#include "testscenario.h"

void alloc_time_series(Opts *opts)
{
  opts->ts = calloc(opts->L, sizeof(double));
  opts->ref_ts = calloc(opts->d, sizeof(double *));
  for (int i = 0; i < opts->d; ++i) {
    opts->ref_ts[i] = calloc(opts->L, sizeof(double));
  }
}


void free_time_series(Opts *opts)
{
  for (int i = 0; i < opts->d; ++i) {
    free(opts->ref_ts[i]);
  }
  free(opts->ref_ts);
  free(opts->ts);
}


double **read_data()
{
  double **data = calloc(ROWS, sizeof(double *));
  for (int i = 0; i < ROWS; ++i) {
    data[i] = calloc(COLS, sizeof(double));
  }

  FILE *file = fopen(FILENAME, "r");
  if (file == NULL) {
    printf("Could not open dataset: ");
    printf("%s\n", FILENAME);
    exit(-1);
  }

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      double value;
      if (!fscanf(file, "%lf", &value)) {}
      data[i][j] = value;
    }
  }

  return data;
}


void free_data(double **data)
{
  for (int i = 0; i < ROWS; ++i) {
    free(data[i]);
  }
  free(data);
}


int main()
{
  Opts opts = {
    .k = PARAM_K,
    .l = PARAM_L,
    .d = PARAM_D,
    .L = (ROWS - MISSING),
    .offset = -1,
  };

  alloc_time_series(&opts);
  double **data = read_data();

#ifdef RUNTIME_TEST
  struct timeval tv1, tv2;
#ifndef RUNTIME_STREAMING_TEST
  (void)gettimeofday (&tv1, NULL);
#endif
#else
  double meansquare = 0.0;
  int meansquare_cnt = 0;
#endif
  
  for (int i = 0; i < ROWS; ++i) {
    opts.offset = mod(opts.offset + 1, opts.L);
    opts.ts[opts.offset] = data[i][0];
    opts.ref_ts[0][opts.offset] = data[i][1];
    opts.ref_ts[1][opts.offset] = data[i][2];
    opts.ref_ts[2][opts.offset] = data[i][3];

#ifdef RUNTIME_STREAMING_TEST
    if (i == opts.L) {
      (void)gettimeofday (&tv1, NULL);
    }
#endif

    if (i >= opts.L) {
#ifndef RUNTIME_TEST
      double real_value = opts.ts[opts.offset];
#endif

      TKCM(&opts);
	  
#ifndef RUNTIME_TEST
	  //mean square
	  double err = real_value - opts.ts[opts.offset];
	  err = err > 0 ? err : -1 * err;
	  meansquare += err * err;
	  ++meansquare_cnt;

	  // outputs:
	  
	  //printf("%lf\t%lf\n", real_value, opts.ts[opts.offset]);
      //printf("%lf\n", opts.ts[opts.offset]);
      //printf("%lf\n", real_value);
	  //printf("%lf\n", err);
	  
	  printf("%d\t%lf\t%lf\t%lf\n", meansquare_cnt, real_value, opts.ts[opts.offset], err);
#endif
    }
  }

#ifdef RUNTIME_TEST
  (void)gettimeofday (&tv2, NULL);
  uint64_t res = (uint64_t) (tv2.tv_sec * 1000000 + tv2.tv_usec) - (uint64_t) (tv1.tv_sec * 1000000 + tv1.tv_usec);
#endif

  FILE *fileout = fopen(FILENAME_OUT, "w");
  
#ifndef RUNTIME_TEST
  // msqerr out
  meansquare /= meansquare_cnt;
  printf("Real\t|\tRecovered\n");
  printf("Mean square error on %d points: %lf\n", meansquare_cnt, meansquare);
  
  opts.offset = -1;
  
  for (int i = 0; i < ROWS; i++) {
    opts.offset = mod(opts.offset + 1, opts.L);

    if (i >= opts.L) {
      printf("idx: %d, offset: %d\n", i, opts.offset);
      fprintf(fileout, "%lf ", opts.ts[opts.offset]);
    } else {
      fprintf(fileout, "%lf ", data[i][0]);
    }
    
    for (int j = 1; j < COLS - 1; j++) {
      fprintf(fileout, "%lf ", data[i][j]);
    }
    fprintf(fileout, "%lf \n", data[i][COLS-1]);
  }
#else
  fprintf(fileout, "%lf", ((double)res) / 1000);
#endif

  fclose(fileout);


  free_time_series(&opts);
  free_data(data);
}
