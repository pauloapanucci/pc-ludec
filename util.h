#ifndef HEADER_UTIL
#define HEADER_UTIL

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "pthread_barrier.h"

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

void printMatrixU(double **m, int n);

void printMatrix(double **m, int n);

void aloc2Dmatrix(double ***m, int l, int c);

void populate2Dmatrix(double **m, int size);

void random2Dmatrix(double **m, int l, int c);

#endif
