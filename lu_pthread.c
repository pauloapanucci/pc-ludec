/**
* lu.c: This file is part of the PolyBench/C 3.2 test suite.
*
*
* Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
* Web address: http://polybench.sourceforge.net
*/

#include "util.h"

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 1024. */
#include "lu.h"

pthread_barrier_t barrier;
pthread_mutex_t lock;

double **I, **S, **Aux;
int size;
int cut;
int nthreads;

/* Array initialization. */

static void init_array(int n, DATA_TYPE POLYBENCH_2D(A,N,N,n,n)){
  I = (double **)malloc(n * sizeof(double));
  S = (double **)calloc(n, sizeof(double));
  Aux = (double **)malloc(n * sizeof(double));
  int i, j;
  for (i = 0; i < n; i++){
    I[i] = (double*)malloc(n * sizeof(double));
    S[i] = (double*)calloc(n, sizeof(double));
    Aux[i] = (double*)malloc(n * sizeof(double));
    for (j = 0; j <= i; j++){
      A[i][j] = (DATA_TYPE)(-j % n) / n + 1;
      I[i][j] = (DATA_TYPE)(-j % n) / n + 1;
      S[i][j] = (DATA_TYPE)(-j % n) / n + 1;
    }
    for (j = i+1; j < n; j++) {
      A[i][j] = 0;
      I[i][j] = 0;
      S[i][j] = 0;
    }
    A[i][i] = 1;
    I[i][i] = 1;
    S[i][i] = 1;
  }

  /* Make the matrix positive semi-definite. */
  int r,s,t;

  for (r = 0; r < n; ++r)
    for (s = 0; s < n; ++s)
      Aux[r][s] = 0;
  for (t = 0; t < n; ++t)
    for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
        Aux[r][s] += I[r][t] * I[s][t];
  for (r = 0; r < n; ++r)
    for (s = 0; s < n; ++s){
      I[r][s] = Aux[r][s];
      S[r][s] = Aux[r][s];
    }
  free2D(Aux);
}


/* DCE code. Must scan the entire live-out data.
Can be used also to check the correctness of the output. */
static void print_array(int n, DATA_TYPE POLYBENCH_2D(A,N,N,n,n)){
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      fprintf (stderr, DATA_PRINTF_MODIFIER, A[i][j]);
      if ((i * n + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}

static void lu_seq(){
  int i, j, k;

  #pragma scop
  for (k = 0; k < size; k++){
    for (j = k+1; j < size; j++){
      S[k][j] = S[k][j] / S[k][k];
    }
    for(i = k+1; i < size; i++){
      for (j = k + 1; j < size; j++){
        S[i][j] -= S[i][k] * S[k][j];
      }
    }
  }
  #pragma endscop
}


/* Main computational kernel. The whole function will be timed,
including the call and return. */
static void *kernel_lu(void *arg){
  int i, j, k;

  // int id = *((int *)arg);
  // int start = (id * size)/nthreads;
  // int end = start + (size/nthreads) - 1;

  int id = *((int *)arg);
  int start = id * cut;
  int end = minimum(start + cut, size);

  #pragma scop
  for (k = 0; k < size; k++){
    int goof = maximum(k + 1, start);
    for (i = goof; i < end; i++){
      I[i][k] /= I[k][k];
    }
    pthread_barrier_wait(&barrier);
    for(i = goof; i < end; i++){
      for (j = k + 1; j < size; j++){
        I[i][j] -= I[i][k] * I[k][j];
      }
    }
    pthread_barrier_wait(&barrier);
  }
  #pragma endscop
  return NULL;

}


void ludec_pthread(){

  pthread_barrier_init(&barrier, NULL, nthreads);

  pthread_t thread[nthreads];

  int arg[nthreads];

  for (int i = 0; i < nthreads; i++) {
    arg[i] = i;
    pthread_create(&thread[i], NULL, kernel_lu, &arg[i]);
  }

  for (int i = 0; i < nthreads; i++) {
    pthread_join(thread[i], NULL);
  }
}


int main(int argc, char** argv){
  srand(time(0));

  /* Retrieve problem size. */

  nthreads = atoi(argv[1]);

  size = N;

  cut = (int)ceil(((float)size) / nthreads);

  /* Variable declaration/allocation. */
  // POLYBENCH_2D_ARRAY_DECL(A, DATA_TYPE, N, N, size, size);


  /* Initialize array(s). */
  // init_array(size, POLYBENCH_ARRAY(A));

  aloc2Dmatrix(&I, size, size);
  populate2Dmatrix(I, size);

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  BEGINTIME();
  ludec_pthread();
  ENDTIME();

  printMatrix(I, size);
  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
  by the function call in argument. */
  // polybench_prevent_dce(print_array(size, POLYBENCH_ARRAY(A)));

  /* Be clean. */
  // POLYBENCH_FREE_ARRAY(A);
  free2D(I);
  // free2D(S);

  return 0;
}
