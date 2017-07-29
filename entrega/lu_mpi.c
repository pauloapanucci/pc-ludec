#include <mpi.h>
#include "util.h"
#include "lu.h"

int size;
int cut;
int nthreads;
int world_size, world_rank;
double **I;
double start, end;

void ludec_mpi(){

  MPI_Status status;


  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  /*Distribute tasks*/
  if(world_rank == 0){
    populate2Dmatrix(I, size);

    for(int i = 1; i < world_size; i++){
      for(int j = 0; j < size; j++){
        for(int k = 0; k < size; k++){
          MPI_Send(&I[j][k], 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }
      }
    }

    // printf("world_rank(%d) distributing the matrix: \n", world_rank);
    // printMatrix(I, size);
  }
  /*Receiving tasks*/
  else{
    for(int i = 0; i < size; i++){
      for(int j = 0; j < size; j++){
        MPI_Recv(&I[i][j], 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    // printf("world_rank(%d) receiving the matrix: \n", world_rank);
    // printMatrix(I, size);
  }
  start = MPI_Wtime();
  /*Executing Kernel*/
  for(int k = 0; k < size; k++) {
    MPI_Bcast(I[k], size, MPI_DOUBLE, k % world_size, MPI_COMM_WORLD);
    for(int i = k + 1; i < size; i++) {
      if(i % world_size == world_rank){
        double l = I[i][k] / I[k][k];
        for (int j = k; j < size; j++){
          I[i][j] -= l * I[k][j];
        }
      }
    }
  }
  end = MPI_Wtime();
  /*Distributing Results*/
  int param = (size % world_size - 1 + world_size ) % world_size;
  if(param != 0) {
    if(world_rank == param) {
      for (int i = 0; i < size; i++){
        MPI_Send(I[i], size, MPI_DOUBLE, 0, size + 1, MPI_COMM_WORLD);
      }
    }
    if(world_rank == 0) {
      for(int i = 0; i < size; i++){
        MPI_Recv(I[i], size, MPI_DOUBLE, param, size + 1, MPI_COMM_WORLD, &status);
      }
    }
  }
}


int main(int argc, char** argv){
  MPI_Init(NULL, NULL);

  size = N;
  aloc2Dmatrix(&I, size, size);

  start = MPI_Wtime();
  ludec_mpi();
  end = MPI_Wtime();

  // if(world_rank == 0){
  //   printMatrix(I, size);
  // }
  //
  // printf("%f\n", end - start);

  free2D(I);

  MPI_Finalize();

  return 0;
}
