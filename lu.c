#include "util.h"
#include "lu.h"

double **I;
int size;

static void kernel_lu(){
  int i, j, k;

  for (k = 0; k < size; k++){
    for (j = k+1; j < size; j++){
      I[k][j] = I[k][j] / I[k][k];
    }
    for(i = k+1; i < size; i++){
      for (j = k + 1; j < size; j++){
        I[i][j] -= I[i][k] * I[k][j];
      }
    }
  }
  
}


int main(int argc, char** argv){
  size = N;

  aloc2Dmatrix(&I, size, size);
  populate2Dmatrix(I, size);

  BEGINTIME();
  kernel_lu();
  ENDTIME();

  // printMatrix(I, size);

  free2D(I);

  return 0;

}
