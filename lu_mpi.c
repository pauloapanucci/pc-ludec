/**
* lu.c: This file is part of the PolyBench/C 3.2 test suite.
*
*
* Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
* Web address: http://polybench.sourceforge.net
*/
#include <mpi.h>
#include "util.h"

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 1024. */
#include "lu.h"

int size;
int cut;
int nthreads;
int world_size, world_rank;
double **I;

void ludec_mpi(){
  size = N;
  MPI_Status status;
  aloc2Dmatrix(&I, size, size);
  populate2Dmatrix(I, size);
  MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  aloc2Dmatrix(&I, size, size);
  populate2Dmatrix(I, size);

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
  /*Print Result*/
  if(world_rank == 0){
    printMatrix(I, size);
  }

	MPI_Finalize();
}


int main(int argc, char** argv){
  /* Retrieve problem size. */

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  // BEGINTIME();
  ludec_mpi();
  // printMatrix(I, size);
  // ENDTIME();

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  return 0;
}
