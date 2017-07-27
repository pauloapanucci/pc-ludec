#ifndef HEADER_UTIL
#define HEADER_UTIL

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
// #include "pthread_barrier.h"

#define maximum(x, y) (((x) > (y)) ? (x) : (y))
#define minimum(x, y) (((x) < (y)) ? (x) : (y))

#define BEGINTIME()\
  struct timespec start, finish;\
  double elapsed_time;\
  clock_gettime(CLOCK_MONOTONIC, &start);



#define ENDTIME()\
  clock_gettime(CLOCK_MONOTONIC, &finish);\
  elapsed_time = (finish.tv_sec - start.tv_sec);\
  elapsed_time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;\
  printf("%.2f\n", elapsed_time);


void free2D(double **m);

void free1D(double *m);

void printMatrixU(double **m, int n);

void printMatrix(double **m, int n);

void printArray(double *a, int n);

void aloc1Dmatrix(double **a, int size);

void aloc2Dmatrix(double ***m, int l, int c);

void populate1Dmatrix(double *a, int nreq, int world_rank, int size);

void populate2Dmatrix(double **m, int size);

void random2Dmatrix(double **m, int l, int c);

#endif
