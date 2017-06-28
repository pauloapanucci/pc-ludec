#include <time.h>
#include <stdlib.h>

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


void free2D(double **m){
  free(*m);
  m = 0;
}

void printMatrixU(double **m, int n){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n; j++){
			printf("%.2f ", m[i][j]);
		}
		printf("\n");
	}
  printf("\n");
}

void printMatrix(double **m, int n){
	int i, j;
  for (i = 0; i < n; i++)
  for (j = 0; j <= i; j++) {
    if ((i * n + j) % 20 == 0) printf("\n");
    printf("%.2f ",m[i][j]);
  }
  printf("\n");
}
