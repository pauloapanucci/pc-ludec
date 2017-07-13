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

// double **Original, **Copy;
int size;
int cut;
int nthreads;
int world_size, world_rank;


void ludec_mpi(){
  size = N;
  MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	double **I;
  aloc2Dmatrix(&I, size, size);

  /*Distribute tasks*/
	if(world_rank == 0){
    populate2Dmatrix(I, size);

		for(int i = 1; i < world_size; i++){
			for(int j = 0; j < size; j++){
        for (int k = 0; k < size; k++) {
          MPI_Send(&I[j][k], 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }
      }
		}

    // printf("world_rank(%d) distributing the matrix: \n", world_rank);
    // printMatrix(Original, size);
	}
  /*Receiving tasks*/
  else{
    for(int i = 0; i < size; i++){
      for (int j = 0; j < size; j++) {
        MPI_Recv(&I[i][j], 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    // printf("world_rank(%d) receiving the matrix: \n", world_rank);
    // printMatrix(Copy, size);
	}

  // /*Executing kernel*/
  int *cut = malloc(size * (sizeof *cut));

  for (int i = 0; i < size; i++){
    cut[i] = i % world_size;
  }

  for (int j = 0; j < size - 1; j++){

    if(cut[j] == world_rank){
      for (int i = j + 1; i < size; i++){
        I[i][j] /= I[j][j];
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /* Brodcast Message for the other processing nodes */
    MPI_Bcast(&I[j][j], size - j - 1, MPI_DOUBLE, cut[j], MPI_COMM_WORLD);

    // for(int i = 0; i < world_size; ++i){
    //   if(i != world_rank){
    //     for (int k = 0; k < j; ++k){
    //        MPI_Send(&I[k][k], 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
    //     }
    //   }
    // }

    // for(int i = 0; i < world_size; ++i){
    //   for (int k = 0; k < j; ++k){
    //      MPI_Recv(&I[k][k], 1, MPI_DOUBLE, cut[j], 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //   }
    // }

    for (int k = j + 1; k < size; k++){
      if(cut[j] == world_rank){
        for (int i = j + 1; i < size; i++){
          I[k][i] -= I[k][j] * I[j][i];
        }
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);

  }

  if(world_rank == 0){
    printMatrix(I, size);
  }

  free2D(I);

	MPI_Finalize();
}


int main(int argc, char** argv){
  /* Retrieve problem size. */

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  // BEGINTIME();
  ludec_mpi();
  // ENDTIME();

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  return 0;
}
