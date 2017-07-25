#include "util.h"
#include "lu.h"

pthread_barrier_t barrier;
pthread_mutex_t lock;

double **I;
int size;
int cut;
int nthreads;


static void *kernel_lu(void *arg){
  int i, j, k;

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

  nthreads = atoi(argv[1]);

  size = N;

  cut = (int)ceil(((float)size) / nthreads);

  aloc2Dmatrix(&I, size, size);
  populate2Dmatrix(I, size);

  BEGINTIME();
  ludec_pthread();
  ENDTIME();

  // printMatrix(I, size);

  free2D(I);

  return 0;
}
