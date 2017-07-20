#include <stdio.h>
#include <stdlib.h> 
#include <mpi.h>
// #include "util.h"

/* Include polybench common header. */
// #include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 1024. */
// #include "lu.h"

int size;
int cut;
int nthreads;
int world_size, world_rank, rp_rank, rp_size, grid_rank, grid_size;
int color;
double **I;

// int main(){
// 	/*Initialize MPI*/
// 	MPI_Init(NULL, NULL);
// 	/*Initialize MPI world rank and size variables*/
// 	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
// 	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
// 	/*Define communicator color*/
// 	int color = world_rank/3;
// 	/*split in row using color*/
// 	MPI_Comm row_comm;
// 	MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);
// 	/*Get the rank and the size by row communicator*/
// 	MPI_Comm_rank(row_comm, &row_rank);
// 	MPI_Comm_size(row_comm, &row_size);

// 	printf("WORLD RANK/SIZE: %d|%d --- ROW RANK/SIZE: %d|%d\n", world_rank, world_size, row_rank, row_size);

// 	MPI_Comm_free(&row_comm);
// 	MPI_Finalize();
// }

int main(){
	/*Initialize MPI*/
	MPI_Init(NULL, NULL);
	/*Initialize MPI world rank and size variables*/
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	/*Define communicator color*/
	if(world_rank/4 == 0){
		color = 0;
	}
	else{
		color = 1;
	}

	/*split in row using color*/
	MPI_Comm grid_comm;
	MPI_Comm request_process_comm;
	if(color == 0){
		MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &request_process_comm);
	}
	else{
		MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &grid_comm);
	}
	/*Get the rank and the size by communicator*/
	if(color == 0){
		MPI_Comm_rank(request_process_comm, &rp_rank);
		MPI_Comm_size(request_process_comm, &rp_size);
	}
	else{
		MPI_Comm_rank(grid_comm, &grid_rank);
		MPI_Comm_size(grid_comm, &grid_size);
	}

	if(color == 0){
		printf("COLOR: %d --- WORLD RANK/SIZE: %d|%d --- RP RANK/SIZE: %d|%d\n", color, world_rank, world_size, rp_rank, rp_size);
	}
	else{
		printf("COLOR: %d --- WORLD RANK/SIZE: %d|%d --- GRID RANK/SIZE: %d|%d\n", color, world_rank, world_size, grid_rank, grid_size);
	}

	if(color == 0){
		MPI_Comm_free(&request_process_comm);
	}
	else{
		MPI_Comm_free(&grid_comm);
	}

	MPI_Finalize();
}