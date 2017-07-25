#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <mpi.h>

int cut;
int nthreads;
int world_size, world_rank,
    gr1_rank, gr2_rank, gr3_rank, gr4_rank,
		gr5_rank, gr6_rank, gr7_rank, gr8_rank,
		gr9_rank, gr10_rank, gr11_rank, gr12_rank,
		req_rank, proc1_rank, proc2_rank, proc3_rank,
		gr1_size, gr2_size, gr3_size, gr4_size,
		gr5_size, gr6_size, gr7_size, gr8_size,
		gr9_size, gr10_size, gr11_size, gr12_size,
		req_size, proc1_size, proc2_size, proc3_size;
int color;
int TAG = 0;

MPI_Group wgroup;
MPI_Group gr1, gr2, gr3, gr4,
					gr5, gr6, gr7, gr8,
					gr9, gr10, gr11, gr12,
					req, proc1, proc2, proc3;
MPI_Comm gr1_comm, gr2_comm, gr3_comm, gr4_comm,
				 gr5_comm, gr6_comm,gr7_comm, gr8_comm,
				 gr9_comm, gr10_comm, gr11_comm, gr12_comm,
				 req_comm, proc1_comm, proc2_comm, proc3_comm;
MPI_Request req_up, req_down, req_left, req_right;
// up, down, left, right
MPI_Comm comms[9][4];
MPI_Request request[4];
// MPI_Comm comms[9][4] = {{gr7_comm, MPI_COMM_NULL, MPI_COMM_NULL, gr1_comm}, {gr8_comm, MPI_COMM_NULL, gr1_comm, gr7_comm}, {gr9_comm, MPI_COMM_NULL, gr2_comm, proc1_comm},
//                         {gr10_comm, gr7_comm, MPI_COMM_NULL, gr3_comm}, {gr11_comm, gr8_comm, gr3_comm, gr4_comm}, {gr12_comm, gr9_comm, gr4_comm, MPI_COMM_NULL},
// 											  {MPI_COMM_NULL, gr10_comm, proc3_comm, gr5_comm}, {MPI_COMM_NULL, gr11_comm, gr5_comm, gr6_comm}, {MPI_COMM_NULL, gr12_comm, gr6_comm, proc2_comm}};
//
// MPI_Request request[4]; = {req_up, req_down, req_left, req_right};
MPI_Status status;


void kernel_lu(int size, double **A){
	int i, j, k;
	for (k = 0; k < size; k++){
		for (j = k+1; j < size; j++){
			A[k][j] = A[k][j] / A[k][k];
		}
		for(i = k+1; i < size; i++){
			for (j = k + 1; j < size; j++){
				A[i][j] -= A[i][k] * A[k][j];
			}
		}
	}
}

void init(){
  request[0] = req_up;
  request[1] = req_down;
  request[2] = req_left;
  request[3] = req_right;

  comms[0][0] = gr7_comm;
  comms[0][1] = MPI_COMM_NULL;
  comms[0][2] = req_comm;
  comms[0][3] = gr1_comm;

  comms[1][0] = gr8_comm;
  comms[1][1] = MPI_COMM_NULL;
  comms[1][2] = gr1_comm;
  comms[1][3] = gr2_comm;

  comms[2][0] = gr9_comm;
  comms[2][1] = MPI_COMM_NULL;
  comms[2][2] = gr2_comm;
  comms[2][3] = proc1_comm;

  comms[3][0] = gr10_comm;;
  comms[3][1] = gr7_comm;
  comms[3][2] = MPI_COMM_NULL;
  comms[3][3] = gr3_comm;

  comms[4][0] = gr11_comm;
  comms[4][1] = gr8_comm;
  comms[4][2] = gr3_comm;
  comms[4][3] = gr4_comm;

  comms[5][0] = gr12_comm;
  comms[5][1] = gr9_comm;
  comms[5][2] = gr4_comm;
  comms[5][3] = MPI_COMM_NULL;

  comms[6][0] = MPI_COMM_NULL;
  comms[6][1] = gr10_comm;
  comms[6][2] = proc3_comm;
  comms[6][3] = gr5_comm;

  comms[7][0] = MPI_COMM_NULL;
  comms[7][1] = gr11_comm;
  comms[7][2] = gr5_comm;
  comms[7][3] = gr6_comm;

  comms[8][0] = MPI_COMM_NULL;
  comms[8][1] = gr12_comm;
  comms[8][2] = gr6_comm;
  comms[8][3] = proc2_comm;
}

/*GROUP METHODS*/

void create_groups(){
	MPI_Comm_group(MPI_COMM_WORLD, &wgroup);

	int ranks[16][2] = {{0, 1}, {1, 2}, {3, 4}, {4, 5},
	                    {6, 7}, {7, 8}, {0, 3}, {1, 4},
	                    {2, 5}, {3, 6}, {4, 7}, {5, 8},
	                    {0, 9}, {2, 10}, {8, 11}, {6, 12}};

	/*INCLUDING GRID GROUP*/
	MPI_Group_incl(wgroup, 2, ranks[0], &gr1);
	MPI_Group_incl(wgroup, 2, ranks[1], &gr2);
	MPI_Group_incl(wgroup, 2, ranks[2], &gr3);
	MPI_Group_incl(wgroup, 2, ranks[3], &gr4);
	MPI_Group_incl(wgroup, 2, ranks[4], &gr5);
	MPI_Group_incl(wgroup, 2, ranks[5], &gr6);
	MPI_Group_incl(wgroup, 2, ranks[6], &gr7);
	MPI_Group_incl(wgroup, 2, ranks[7], &gr8);
	MPI_Group_incl(wgroup, 2, ranks[8], &gr9);
	MPI_Group_incl(wgroup, 2, ranks[9], &gr10);
	MPI_Group_incl(wgroup, 2, ranks[10], &gr11);
	MPI_Group_incl(wgroup, 2, ranks[11], &gr12);
	/*INCLUDING REQUEST GROUP*/
	MPI_Group_incl(wgroup, 2, ranks[12], &req);
	/*INCLUDING PROCESS GROUP*/
	MPI_Group_incl(wgroup, 2, ranks[13], &proc1);
	MPI_Group_incl(wgroup, 2, ranks[14], &proc2);
	MPI_Group_incl(wgroup, 2, ranks[15], &proc3);
	/*CREATING GRID GROUP*/
	MPI_Comm_create_group(MPI_COMM_WORLD, gr1, 0, &gr1_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr2, 0, &gr2_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr3, 0, &gr3_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr4, 0, &gr4_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr5, 0, &gr5_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr6, 0, &gr6_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr7, 0, &gr7_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr8, 0, &gr8_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr9, 0, &gr9_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr10, 0, &gr10_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr11, 0, &gr11_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, gr12, 0, &gr12_comm);
	/*CREATING REQUEST GROUP*/
	MPI_Comm_create_group(MPI_COMM_WORLD, req, 0, &req_comm);
	/*CREATING PROCESS GROUP*/
	MPI_Comm_create_group(MPI_COMM_WORLD, proc1, 0, &proc1_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, proc2, 0, &proc2_comm);
	MPI_Comm_create_group(MPI_COMM_WORLD, proc3, 0, &proc3_comm);
}

void free_groups(){
	if (MPI_COMM_NULL != gr1_comm) {
		MPI_Group_free(&gr1);
		MPI_Comm_free(&gr1_comm);
	}
	else if(MPI_COMM_NULL != gr2_comm){
		MPI_Group_free(&gr2);
		MPI_Comm_free(&gr2_comm);
	}
	else if(MPI_COMM_NULL != gr3_comm){
		MPI_Group_free(&gr3);
		MPI_Comm_free(&gr3_comm);
	}
	else if(MPI_COMM_NULL != gr4_comm){
		MPI_Group_free(&gr4);
		MPI_Comm_free(&gr4_comm);
	}
	else if(MPI_COMM_NULL != gr5_comm){
		MPI_Group_free(&gr5);
		MPI_Comm_free(&gr5_comm);
	}
	else if(MPI_COMM_NULL != gr6_comm){
		MPI_Group_free(&gr6);
		MPI_Comm_free(&gr6_comm);
	}
	else if(MPI_COMM_NULL != gr7_comm){
		MPI_Group_free(&gr7);
		MPI_Comm_free(&gr7_comm);
	}
	else if(MPI_COMM_NULL != gr8_comm){
		MPI_Group_free(&gr8);
		MPI_Comm_free(&gr8_comm);
	}
	else if(MPI_COMM_NULL != gr9_comm){
		MPI_Group_free(&gr9);
		MPI_Comm_free(&gr9_comm);
	}
	else if(MPI_COMM_NULL != gr10_comm){
		MPI_Group_free(&gr10);
		MPI_Comm_free(&gr10_comm);
	}
	else if(MPI_COMM_NULL != gr11_comm){
		MPI_Group_free(&gr11);
		MPI_Comm_free(&gr11_comm);
	}
	else if(MPI_COMM_NULL != gr12_comm){
		MPI_Group_free(&gr12);
		MPI_Comm_free(&gr12_comm);
	}
	else if(MPI_COMM_NULL != req_comm){
		MPI_Group_free(&req);
		MPI_Comm_free(&req_comm);
	}
	else if(MPI_COMM_NULL != proc1_comm){
		MPI_Group_free(&proc1);
		MPI_Comm_free(&proc1_comm);
	}
	else if(MPI_COMM_NULL != proc2_comm){
		MPI_Group_free(&proc2);
		MPI_Comm_free(&proc2_comm);
	}
	else if(MPI_COMM_NULL != proc3_comm){
		MPI_Group_free(&proc3);
		MPI_Comm_free(&proc3_comm);
	}
}

void test_groups(){
	if (MPI_COMM_NULL != gr1_comm) {
		MPI_Comm_rank(gr1_comm, &gr1_rank);
		MPI_Comm_size(gr1_comm, &gr1_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR1 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr1_rank, gr1_size);
	}
	else if(MPI_COMM_NULL != gr2_comm){
		MPI_Comm_rank(gr2_comm, &gr2_rank);
		MPI_Comm_size(gr2_comm, &gr2_size);
		printf("WORLD RANK/SIZE: %d/%d \t G21 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr2_rank, gr2_size);
	}
	else if(MPI_COMM_NULL != gr3_comm){
		MPI_Comm_rank(gr3_comm, &gr3_rank);
		MPI_Comm_size(gr3_comm, &gr3_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR3 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr3_rank, gr3_size);
	}
	else if(MPI_COMM_NULL != gr4_comm){
		MPI_Comm_rank(gr4_comm, &gr4_rank);
		MPI_Comm_size(gr4_comm, &gr4_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR4 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr4_rank, gr4_size);
	}
	else if(MPI_COMM_NULL != gr5_comm){
		MPI_Comm_rank(gr5_comm, &gr5_rank);
		MPI_Comm_size(gr5_comm, &gr5_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR5 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr5_rank, gr5_size);
	}
	else if(MPI_COMM_NULL != gr6_comm){
		MPI_Comm_rank(gr6_comm, &gr6_rank);
		MPI_Comm_size(gr6_comm, &gr6_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR6 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr6_rank, gr6_size);
	}
	else if(MPI_COMM_NULL != gr7_comm){
		MPI_Comm_rank(gr7_comm, &gr7_rank);
		MPI_Comm_size(gr7_comm, &gr7_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR7 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr7_rank, gr7_size);
	}
	else if(MPI_COMM_NULL != gr8_comm){
		MPI_Comm_rank(gr8_comm, &gr8_rank);
		MPI_Comm_size(gr8_comm, &gr8_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR8 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr8_rank, gr8_size);
	}
	else if(MPI_COMM_NULL != gr9_comm){
		MPI_Comm_rank(gr9_comm, &gr9_rank);
		MPI_Comm_size(gr9_comm, &gr9_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR9 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr9_rank, gr9_size);
	}
	else if(MPI_COMM_NULL != gr10_comm){
		MPI_Comm_rank(gr10_comm, &gr10_rank);
		MPI_Comm_size(gr10_comm, &gr10_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR10 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr10_rank, gr10_size);
	}
	else if(MPI_COMM_NULL != gr11_comm){
		MPI_Comm_rank(gr11_comm, &gr11_rank);
		MPI_Comm_size(gr11_comm, &gr11_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR11 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr11_rank, gr11_size);
	}
	else if(MPI_COMM_NULL != gr12_comm){
		MPI_Comm_rank(gr12_comm, &gr12_rank);
		MPI_Comm_size(gr12_comm, &gr12_size);
		printf("WORLD RANK/SIZE: %d/%d \t GR12 RANK/SIZE: %d/%d\n",
		world_rank, world_size, gr12_rank, gr12_size);
	}
	else if(MPI_COMM_NULL != req_comm){
		MPI_Comm_rank(req_comm, &req_rank);
		MPI_Comm_size(req_comm, &req_size);
		printf("WORLD RANK/SIZE: %d/%d \t REQ RANK/SIZE: %d/%d\n",
		world_rank, world_size, req_rank, req_size);
	}
	else if(MPI_COMM_NULL != proc1_comm){
		MPI_Comm_rank(proc1_comm, &proc1_rank);
		MPI_Comm_size(proc1_comm, &proc1_size);
		printf("WORLD RANK/SIZE: %d/%d \t PROC1 RANK/SIZE: %d/%d\n",
		world_rank, world_size, proc1_rank, proc1_size);
	}
	else if(MPI_COMM_NULL != proc2_comm){
		MPI_Comm_rank(proc2_comm, &proc2_rank);
		MPI_Comm_size(proc2_comm, &proc2_size);
		printf("WORLD RANK/SIZE: %d/%d \t PROC2 RANK/SIZE: %d/%d\n",
		world_rank, world_size, proc2_rank, proc2_size);
	}
	else if(MPI_COMM_NULL != proc3_comm){
		MPI_Comm_rank(proc3_comm, &proc3_rank);
		MPI_Comm_size(proc3_comm, &proc3_size);
		printf("WORLD RANK/SIZE: %d/%d \t PROC3 RANK/SIZE: %d/%d\n",
		world_rank, world_size, proc3_rank, proc3_size);
	}

}

int is_valid_connection(int pos){
  switch (pos) {
    case 0: //case up
      if(world_rank == 6 || world_rank == 7 || world_rank == 8)
        return 0;
      else
        return 1;
    case 1: //case down
      if(world_rank == 0 || world_rank == 1 || world_rank == 2)
        return 0;
      else
        return 1;
    case 2: //case left
      if(world_rank == 3)
        return 0;
      else
        return 1;
    case 3: //case right
      if(world_rank == 5)
        return 0;
      else
        return 1;
    default:
      return 0;

  }
}

int rand_grid(int i){
  srand(time(0));
  int pos = i ;
  if(world_rank == 0 || world_rank == 6){
    do{
      pos = rand() % 4;
    }while((!is_valid_connection(pos) || pos == 2) || pos == i);
  }
  else if(world_rank == 2 || world_rank == 8){
    do{
      pos = rand() % 3;
    }while(!is_valid_connection(pos) || pos == i);
  }
  else{
    do{
      pos = rand() % 4;
    }while(!is_valid_connection(pos) || pos == i);
  }
  return pos;
}

/*GRID LOOPS*/

void request_loop(int nreq){
  int args[3]; // args[req, world_rank, size]
  for(int i = 0; i < nreq; i++) {
    args[0] = i; //req
    args[1] = world_rank; //world_rank
    args[2] = 20; // size
    MPI_Send(&args, 3, MPI_INT, 0, TAG, req_comm);
    double **I;
    aloc2Dmatrix(&I, args[2], args[2]);
    populate2Dmatrix(I, args[2]);
    for (int j = 0; j < args[2]; j++) {
      for (int k = 0; k < args[2]; k++) {
        MPI_Send(&I[j][k], 1, MPI_DOUBLE, 0, TAG, req_comm);
      }
    }
    printf("SEND REQUEST %d FROM WORLD_RANK %d to WORLD_RANK 0 [size: %d]\n", i, world_rank, args[2]);
    // sleep(2);
    free2D(I);
  }
  while (1) {

  }

}

void process_loop(){
  int args[3];
  if (world_rank == 10) {
    while (1) {
      MPI_Recv(&args, 3, MPI_INT, 0, TAG, proc1_comm, &status);
      double **I;
      aloc2Dmatrix(&I, args[2], args[2]);
      for (int j = 0; j < args[2]; j++) {
        for (int k = 0; k < args[2]; k++) {
          MPI_Recv(&I[j][k], 1, MPI_DOUBLE, 0, TAG, proc1_comm, &status);
        }
      }
      printf("\t\t\tPROCESSING REQ %d RECEIVED FROM %d ON PROCESSOR %d [size: %d]\n", args[0], args[1], world_rank, args[2]);
      // MPI_Send(&size, 1, MPI_INT, 0, TAG, proc1_comm);
      kernel_lu(args[2], I);
      printMatrix(I, args[2]);
      free2D(I);
    }
  }
  else if(world_rank == 11){
    while (1) {
      MPI_Recv(&args, 3, MPI_INT, 0, TAG, proc2_comm, &status);
      double **I;
      aloc2Dmatrix(&I, args[2], args[2]);
      for (int j = 0; j < args[2]; j++) {
        for (int k = 0; k < args[2]; k++) {
          MPI_Recv(&I[j][k], 1, MPI_DOUBLE, 0, TAG, proc2_comm, &status);
        }
      }
      printf("\t\t\tPROCESSING REQ %d RECEIVED FROM %d ON PROCESSOR %d [size: %d]\n", args[0], args[1], world_rank, args[2]);
      // MPI_Send(&size, 1, MPI_INT, 0, TAG, proc1_comm);
      kernel_lu(args[2], I);
      printMatrix(I, args[2]);
      free2D(I);
    }
  }
  else if(world_rank == 12){
    while (1) {
      MPI_Recv(&args, 3, MPI_INT, 0, TAG, proc3_comm, &status);
      double **I;
      aloc2Dmatrix(&I, args[2], args[2]);
      for (int j = 0; j < args[2]; j++) {
        for (int k = 0; k < args[2]; k++) {
          MPI_Recv(&I[j][k], 1, MPI_DOUBLE, 0, TAG, proc3_comm, &status);
        }
      }
      printf("\t\t\tPROCESSING REQ %d RECEIVED FROM %d ON PROCESSOR %d [size: %d]\n", args[0], args[1], world_rank, args[2]);
      // MPI_Send(&size, 1, MPI_INT, 0, TAG, proc1_comm);
      kernel_lu(args[2], I);
      printMatrix(I, args[2]);
      free2D(I);
    }
  }
}

void grid_loop(){
  MPI_Request rup, rdown, rleft, rright;
  MPI_Request r[4] = {rup, rdown, rleft, rright};
  MPI_Request rproc;
  MPI_Status rstatus;
  int grank, value, flag;
  int args[3];
  int run = 1;
  while(run){
    for(int i = 0; i < 4; i++) {
      if(is_valid_connection(i)){
        MPI_Comm_rank(comms[world_rank][i], &grank);
        if(grank == 0){
          MPI_Irecv(&args, 3, MPI_INT, 1, TAG, comms[world_rank][i], &r[i]);
        }
        else{
          MPI_Irecv(&args, 3, MPI_INT, 0, TAG, comms[world_rank][i], &r[i]);
        }
        MPI_Test(&r[i], &flag, &rstatus);
        if(flag){
          printf("\tNODE %d RECEIVING PACKAGE FROM %d [req: %d size: %d]\n", world_rank, args[1], args[0], args[2]);
          //RECEBENDO A MATRIX
          double **I;
          aloc2Dmatrix(&I, args[2], args[2]);
          if(grank == 0){
            //CONFIRMA QUE PODE COMEÇAR O ENVIO
            // MPI_Send(1, 1, MPI_INT, 1, TAG, comms[world_rank][i], &rstatus);
            for (int j = 0; j < args[2]; j++) {
              for (int  k = 0; k < args[2]; k++) {
                MPI_Recv(&I[j][k], 1, MPI_DOUBLE, 1, TAG, comms[world_rank][i], &rstatus);
              }
            }
          }
          else{
            //CONFIRMA QUE PODE COMEÇAR O ENVIO
            // MPI_Send(1, 1, MPI_INT, 0, TAG, comms[world_rank][i], &rstatus);
            for (int j = 0; j < args[2]; j++) {
              for (int  k = 0; k < args[2]; k++) {
                MPI_Recv(&I[j][k], 1, MPI_DOUBLE, 0, TAG, comms[world_rank][i], &rstatus);
              }
            }
          }
          // printMatrix(I, args[2]);
          //CASO CANTOS PROCESSADOR
          if(world_rank == 2 || world_rank == 6 || world_rank == 8){
             //VERIFICA SE PROCESSADOR ESTÁ DISPONIVEL
              if(world_rank == 6){
                MPI_Isend(&args, 3, MPI_INT, 1, TAG, comms[world_rank][2], &rproc);
              }
              else{
                MPI_Isend(&args, 3, MPI_INT, 1, TAG, comms[world_rank][3], &rproc);
              }
              MPI_Test(&rproc, &flag, &rstatus);
              if(flag){
                if(world_rank == 2){
                  printf("\t\tPOCESSOR READY %d TO DO THE LUDEC, SEND THE MATRIX!\n", 10);
                }
                else if(world_rank == 6){
                  printf("\t\tPOCESSOR READY %d TO DO THE LUDEC, SEND THE MATRIX!\n", 12);
                }
                else{
                  printf("\t\tPOCESSOR READY %d TO DO THE LUDEC, SEND THE MATRIX!\n", 11);
                }
                if(world_rank == 6){
                  //CONFIRMA QUE PODE COMEÇAR O ENVIO
                  // MPI_Send(1, 1, MPI_INT, 1, TAG, comms[world_rank][i], &rstatus);
                  for (int j = 0; j < args[2]; j++) {
                    for (int k = 0; k < args[2]; k++) {
                      MPI_Send(&I[j][k], 1, MPI_DOUBLE, 1, TAG, comms[world_rank][2]);
                    }
                  }

                }
                else{
                  for (int j = 0; j < args[2]; j++) {
                    for (int k = 0; k < args[2]; k++) {
                      MPI_Send(&I[j][k], 1, MPI_DOUBLE, 1, TAG, comms[world_rank][3]);
                    }
                  }
                }
              }
              else{
                int pos = rand_grid(i);
                MPI_Comm_rank(comms[world_rank][pos], &grank);
                args[1] = world_rank; // modifica o world_rank pra enviar
                printf("SEND REQUEST %d FROM WORLD_RANK %d [size: %d]\n", args[0], world_rank, args[2]);
                if(grank == 0){
                  MPI_Send(&args, 3, MPI_INT, 1, TAG, comms[world_rank][pos]);
                  for (int j = 0; j < args[2]; j++) {
                    for (int k = 0; k < args[2]; k++) {
                      MPI_Send(&I[j][k], 1, MPI_DOUBLE, 1, TAG, comms[world_rank][pos]);
                    }
                  }
                }
                else{
                  MPI_Send(&args, 3, MPI_INT, 0, TAG, comms[world_rank][pos]);
                  for (int j = 0; j < args[2]; j++) {
                    for (int k = 0; k < args[2]; k++) {
                      MPI_Send(&I[j][k], 1, MPI_DOUBLE, 0, TAG, comms[world_rank][pos]);
                    }
                  }
                }
              }
              free2D(I);
          }
          else{ //RAND NO GRID E SIMBORA PRA OUTRO NÓ
            double **I;
            aloc2Dmatrix(&I, args[2], args[2]);
            int pos = rand_grid(i);
            MPI_Comm_rank(comms[world_rank][pos], &grank);
            args[1] = world_rank; // modifica o world_rank pra enviar
            if(grank == 0){
              MPI_Send(&args, 3, MPI_INT, 1, TAG, comms[world_rank][pos]);
              for (int j = 0; j < args[2]; j++) {
                for (int k = 0; k < args[2]; k++) {
                  MPI_Send(&I[j][k], 1, MPI_DOUBLE, 1, TAG, comms[world_rank][pos]);
                }
              }
            }
            else{
              MPI_Send(&args, 3, MPI_INT, 0, TAG, comms[world_rank][pos]);
              for (int j = 0; j < args[2]; j++) {
                for (int k = 0; k < args[2]; k++) {
                  MPI_Send(&I[j][k], 1, MPI_DOUBLE, 0, TAG, comms[world_rank][pos]);
                }
              }
            }
            printf("SEND REQUEST %d FROM WORLD_RANK %d [size: %d]\n", args[0], world_rank, args[2]);
            free2D(I);
          }

        }//endifflag
      }//endifvalidconnection
    }//endfor
  }//endwhilerun
}

int main(int argc, char **argv){
  // if(argc < 2){
  //   printf("pass the quantity of requests as argument. EX: mpirun ./lu_mpi.out 3\n");
  // }
  int nreq = atoi(argv[1]);

	/*Initialize MPI*/
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  create_groups();
  // test_groups();
  init();
	/*REQUEST NODE*/
	if(world_rank == 9){
      request_loop(nreq);
	}
	/*PROCESSING NODES*/
	else if(world_rank >= 10 && world_rank <= 12){
    process_loop();
	}
	/*GRID NODES*/
	else if(world_rank >= 0 && world_rank <= 8){
    grid_loop();
	}
	free_groups();
	MPI_Finalize();
}
