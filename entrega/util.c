#include "util.h"

void free2D(double **m){
  free(*m);
  m = 0;
}

void free1D(double *m){
  free(m);
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
	// int i, j;
  // for (i = 0; i < n; i++)
  // for (j = 0; j <= i; j++) {
  //   if ((i * n + j) % 20 == 0) printf("\n");
  //   printf("%.2f ",m[i][j]);
  // }
  // printf("\n");
  for (int i = 0; i < n; ++i){
    for (int j = 0; j < n; ++j){
      printf("%.2f ",m[i][j]);
    }
    printf("\n");
  }
}

void printArray(double *a, int n){
	for (int i = 4; i < (n * n) + 4; i++) {
    if((i - 4) % n == 0) printf("\n");
	  printf("%.2f ",a[i]);
	}
}

void aloc1Dmatrix(double **a, int size){
  int n = (size * size) + 4;
  (*a) = malloc(n * sizeof **a);
  for (int i = 0; i < n; i++) {
    (*a)[i] = 0;
  }
}


void aloc2Dmatrix(double ***m, int l, int c){
  *m = (double **)malloc(l * sizeof(double*));
  for (int i = 0; i < l; i++) {
    (*m)[i] = (double*)malloc(c * sizeof(double));
    for (int j = 0; j < c; j++) {
      (*m)[i][j] = 0;
    }
  }
}

void populate1Dmatrix(double *a, int flag, int nreq, int world_rank, int size){
  //próprio método do polybench

  int i, j;

  double **m, **aux;
  aloc2Dmatrix(&m, size, size);
  aux = (double **)malloc(size * sizeof(double*));

  for (i = 0; i < size; i++){
    aux[i] = (double*)malloc(size * sizeof(double));
    for (j = 0; j <= i; j++){
      m[i][j] = (double)(-j % size) / size + 1;
    }
    for (j = i+1; j < size; j++) {
      m[i][j] = 0;
    }
    m[i][i] = 1;
  }

  /* Make the matrix positive semi-definite. */
  int r,s,t;

  for (r = 0; r < size; ++r)
    for (s = 0; s < size; ++s)
      aux[r][s] = 0;
  for (t = 0; t < size; ++t)
    for (r = 0; r < size; ++r)
      for (s = 0; s < size; ++s)
        aux[r][s] += m[r][t] * m[s][t];
  for (r = 0; r < size; ++r)
    for (s = 0; s < size; ++s){
      m[r][s] = aux[r][s];
    }

    a[0] = flag;
    a[1] = nreq;
    a[2] = world_rank;
    a[3] = size;
    int k = 4;
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        a[k] = m[i][j];
        k++;
      }
    }

  free2D(m);
  free2D(aux);
}

void populate2Dmatrix(double **m, int size){
  //próprio método do polybench

  int i, j;

  double **aux;
  aux = (double **)malloc(size * sizeof(double*));

  for (i = 0; i < size; i++){
    aux[i] = (double*)malloc(size * sizeof(double));
    for (j = 0; j <= i; j++){
      m[i][j] = (double)(-j % size) / size + 1;
    }
    for (j = i+1; j < size; j++) {
      m[i][j] = 0;
    }
    m[i][i] = 1;
  }

  /* Make the matrix positive semi-definite. */
  int r,s,t;

  for (r = 0; r < size; ++r)
    for (s = 0; s < size; ++s)
      aux[r][s] = 0;
  for (t = 0; t < size; ++t)
    for (r = 0; r < size; ++r)
      for (s = 0; s < size; ++s)
        aux[r][s] += m[r][t] * m[s][t];
  for (r = 0; r < size; ++r)
    for (s = 0; s < size; ++s){
      m[r][s] = aux[r][s];
    }
  free2D(aux);
}

void random2Dmatrix(double **m, int l, int c){
  for (int i = 0; i < l; i++) {
    for (int j = 0; j < c; j++) {
      int n = rand() % 10;
			m[i][j] = n;
    }
  }
}
