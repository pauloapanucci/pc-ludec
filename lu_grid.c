#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <mpi.h>

/*RANKS AND SIZES*/
int world_size, world_rank,
    gr1_rank, gr2_rank, gr3_rank, gr4_rank,
    gr5_rank, gr6_rank, gr7_rank, gr8_rank,
    gr9_rank, gr10_rank, gr11_rank, gr12_rank,
    req_rank, proc1_rank, proc2_rank, proc3_rank,
    gr1_size, gr2_size, gr3_size, gr4_size,
    gr5_size, gr6_size, gr7_size, gr8_size,
    gr9_size, gr10_size, gr11_size, gr12_size,
    req_size, proc1_size, proc2_size, proc3_size;

/*UTILITIES*/
int maxsize, proc_is_busy, procflag, reqorder[4],
    grank, flag[4], TAG = 0, run = 1;

/*GROUPS*/
MPI_Group wgroup;
MPI_Group gr1, gr2, gr3, gr4,
          gr5, gr6, gr7, gr8,
          gr9, gr10, gr11, gr12,
          req, proc1, proc2, proc3;

/*COMMUNICATORS*/
MPI_Comm gr1_comm, gr2_comm, gr3_comm, gr4_comm,
         gr5_comm, gr6_comm,gr7_comm, gr8_comm,
         gr9_comm, gr10_comm, gr11_comm, gr12_comm,
         req_comm, proc1_comm, proc2_comm, proc3_comm;
// up, down, left, right
MPI_Comm comms[9][4];

/*REQUESTS*/
MPI_Request r[4], rproc;

/*STATUS*/
MPI_Status status, rstatus;

/*LU DECOMPOSITION KERNEL*/

void kernel_lu(int size, double *A){
  int i, j, k;
  for (k = 0; k < size; k++){
    for (j = k+1; j < size; j++){
      A[(size * k) + j + 4] = A[(size * k) + j + 4] / A[(size * k) + k + 4];
    }
    for(i = k+1; i < size; i++){
      for (j = k + 1; j < size; j++){
        A[(size * i) + j + 4] -= A[(size * i) + k + 4] * A[(size * k) + j + 4];
      }
    }
  }
}

/*GROUP METHODS*/

void init(){
  proc_is_busy = 0;

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

/*GRID FUNCTIONS*/

int calc_world_rank(int pos){
  /*Calculate world rank that is going to receive the message*/
  switch (pos) {
    case 0:
      if(world_rank == 0) return 3;
      else if(world_rank == 1) return 4;
      else if(world_rank == 2) return 5;
      else if(world_rank == 3) return 6;
      else if(world_rank == 4) return 7;
      else if(world_rank == 5) return 8;
      // else if(world_rank == 6) return 0;
      // else if(world_rank == 7) return 0;
      // else if(world_rank == 8) return 0;
    case 1:
      // if(world_rank == 0) return 0;
      // else if(world_rank == 1) return 0;
      // else if(world_rank == 2) return 0;
      if(world_rank == 3) return 0;
      else if(world_rank == 4) return 1;
      else if(world_rank == 5) return 2;
      else if(world_rank == 6) return 3;
      else if(world_rank == 7) return 4;
      else if(world_rank == 8) return 5;
    case 2:
      // if(world_rank == 0) return 0;
      if(world_rank == 1) return 0;
      else if(world_rank == 2) return 1;
      // else if(world_rank == 3) return 0;
      else if(world_rank == 4) return 3;
      else if(world_rank == 5) return 4;
      // else if(world_rank == 6) return 0;
      else if(world_rank == 7) return 6;
      else if(world_rank == 8) return 7;
    case 3:
      if(world_rank == 0) return 1;
      else if(world_rank == 1) return 2;
      // else if(world_rank == 2) return 0;
      else if(world_rank == 3) return 4;
      else if(world_rank == 4) return 5;
      // else if(world_rank == 5) return 0;
      else if(world_rank == 6) return 7;
      else if(world_rank == 7) return 8;
      // else if(world_rank == 8) return 0;
  }
  return -1;
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

void request_loop(int *reqs, int nreqs, int maxsize){
  int n = (maxsize * maxsize) + 4;
  for(int i = 0; i < nreqs; i++) {
    double *args;
    aloc1Dmatrix(&args, maxsize);
    populate1Dmatrix(args, 0, i + 1, world_rank, reqs[i]);
    printf("REQUISITOR (WR: %d) - ENVIANDO REQUISIÇÃO [numero de requisição: %d tamanho %d]\n", world_rank, i + 1, reqs[i]);
    MPI_Send(args, n, MPI_DOUBLE, 0, TAG, req_comm);
    free(args);
  }
  for(int i = 0; i < nreqs; i++) {
    double *args;
    aloc1Dmatrix(&args, maxsize);
    MPI_Recv(args, n, MPI_DOUBLE, 0, TAG, req_comm, &status);
    printf("REQUISITOR (WR: %d) - REQUISIÇÃO ATENDIDA [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
    free(args);
  }
  double *args;
  aloc1Dmatrix(&args, maxsize);
  args[0] = 2;
  printf("REQUISITOR (WR: %d) - TODAS AS REQUISIÇÕES FORAM ATENDIDAS - ENVIANDO MENSAGEM DE FIM\n", world_rank);
  MPI_Send(args, n, MPI_DOUBLE, 0, TAG, req_comm);
  free(args);
  printf("REQUISITOR (WR: %d) - TODAS AS REQUISIÇÕES FORAM ATENDIDAS - PARANDO AS ATIVIDADES NO REQUISITOR\n", world_rank);
}

void process_loop(){
  int n = (maxsize * maxsize) + 4;
  double* args = malloc(n * sizeof *args);
  if (world_rank == 10) {
    while (run) {
      MPI_Recv(args, n, MPI_DOUBLE, 0, TAG, proc1_comm, &status);
      if(args[0] != 2){
        printf("PROCESSADOR (WR: %d) - PROCESSANDO REQUISIÇÃO [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
        kernel_lu(args[3], args);
        args[0] = 1;
        MPI_Send(args, n, MPI_DOUBLE, 0, TAG, proc1_comm);
        // proc_is_busy = 0;
      }
      else {
        printf("PROCESSADOR (WR: %d) - TODAS AS REQUISIÇÕES FORAM PROCESSADAS - PARANDO AS ATIVIDADES NO PROCESSADOR\n", world_rank);
        run = 0;
      }
    }
  }
  else if(world_rank == 11){
    while (run) {
      MPI_Recv(args, n, MPI_DOUBLE, 0, TAG, proc2_comm, &status);
      if(args[0] != 2){
        printf("PROCESSADOR (WR: %d) - PROCESSANDO REQUISIÇÃO [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
        kernel_lu(args[3], args);
        args[0] = 1;
        MPI_Send(args, n, MPI_DOUBLE, 0, TAG, proc2_comm);
        // proc_is_busy = 0;
      }
      else {
        printf("PROCESSADOR (WR: %d) - TODAS AS REQUISIÇÕES FORAM PROCESSADAS - PARANDO AS ATIVIDADES NO PROCESSADOR\n", world_rank);
        run = 0;
      }
    }
  }
  else if(world_rank == 12){
    while (run) {
      MPI_Recv(args, n, MPI_DOUBLE, 0, TAG, proc3_comm, &status);
      if (args[0] != 2) {
        printf("PROCESSADOR (WR: %d) - PROCESSANDO REQUISIÇÃO [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
        kernel_lu(args[3], args);
        args[0] = 1;
        MPI_Send(args, n, MPI_DOUBLE, 0, TAG, proc3_comm);
        // proc_is_busy = 0;
      }
      else {
        printf("PROCESSADOR (WR: %d) - TODAS AS REQUISIÇÕES FORAM PROCESSADAS - PARANDO AS ATIVIDADES NO PROCESSADOR\n", world_rank);
        run = 0;
      }
    }
  }
}

void grid_loop_go(double *args, int j, int n){
  printf("GRID (WR: %d) - RECEBENDO REQUISIÇÃO DE %.0f [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[2], args[1], args[3]);
  //CASO CANTOS PROCESSADOR
  if(world_rank == 2 || world_rank == 6 || world_rank == 8){
    //VERIFICA SE PROCESSADOR ESTÁ DISPONIVEL
    if(!proc_is_busy){
      if(world_rank == 2){
        printf("GRID (WR: %d) - PROCESSADOR 10 PRONTO PARA ATENDER A REQUISIÇÃO [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
      }
      else if(world_rank == 6){
        printf("GRID (WR: %d) - PROCESSADOR 11 PRONTO PARA ATENDER A REQUISIÇÃO [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
      }
      else{
        printf("GRID (WR: %d) - PROCESSADOR 12 PRONTO PARA ATENDER A REQUISIÇÃO [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
      }
      if(world_rank == 6){
        MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][2]);
      }
      else{
        MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][3]);
      }
      proc_is_busy = 1;
    }
    else{
      int pos = rand_grid(j);
      MPI_Comm_rank(comms[world_rank][pos], &grank);
      args[2] = world_rank; // modifica o world_rank pra enviar
      if(world_rank == 2){
        printf("GRID (WR: %d) - PARECE QUE O PROCESSADOR 10 ESTA OCUPADO. RETRANSMITINDO REQUISIÇÃO PARA %d [numero de requisição: %.0f tamanho %.0f]\n", world_rank, calc_world_rank(pos), args[1], args[3]);
      }
      else if(world_rank == 6){
        printf("GRID (WR: %d) - PARECE QUE O PROCESSADOR 12 ESTA OCUPADO. RETRANSMITINDO REQUISIÇÃO PARA %d [numero de requisição: %.0f tamanho %.0f]\n", world_rank, calc_world_rank(pos), args[1], args[3]);
      }
      else{
        printf("GRID (WR: %d) - PARECE QUE O PROCESSADOR 11 ESTA OCUPADO. RETRANSMITINDO REQUISIÇÃO PARA %d [numero de requisição: %.0f tamanho %.0f]\n", world_rank, calc_world_rank(pos), args[1], args[3]);
      }
      if(grank == 0){
        MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][pos]);
      }
      else{
        MPI_Send(args, n, MPI_DOUBLE, 0, TAG, comms[world_rank][pos]);
      }
    }
  }
  else{ //RAND NO GRID E SIMBORA PRA OUTRO NÓ
    int pos = rand_grid(j);
    MPI_Comm_rank(comms[world_rank][pos], &grank);
    args[2] = world_rank; // modifica o world_rank pra enviar
    printf("GRID (WR: %d) - RETRANSMITINDO REQUISIÇÃO PARA %d [numero de requisição: %.0f tamanho %.0f]\n", world_rank, calc_world_rank(pos), args[1], args[3]);
    if(grank == 0){
      MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][pos]);
    }
    else{
      MPI_Send(args, n, MPI_DOUBLE, 0, TAG, comms[world_rank][pos]);
    }
  }
  r[j] = MPI_REQUEST_NULL;
}

void grid_loop_back(double *args, int j, int n){
  if(world_rank == 8 || world_rank == 6 || world_rank == 5 || world_rank == 3){
    MPI_Send(args, n, MPI_DOUBLE, 0, TAG, comms[world_rank][1]);
  }
  else if(world_rank == 2 || world_rank == 1){
    MPI_Send(args, n, MPI_DOUBLE, 0, TAG, comms[world_rank][2]);
  }
  else if(world_rank == 0){
    MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][2]);
  }
  if(world_rank == 2 || world_rank == 6 || world_rank == 8){ //libera o processador
    proc_is_busy = 0;
  }
  printf("GRID (WR: %d) - RETORNANDO REQUISIÇÃO ATENDIDA [numero de requisição: %.0f tamanho %.0f]\n", world_rank, args[1], args[3]);
  r[j] = MPI_REQUEST_NULL;
}

void grid_loop_kill(double *args, int j, int n){
  printf("GRID (WR: %d) - PARANDO AS ATIVIDADES NO GRID\n", world_rank);
  if(world_rank == 0 || world_rank == 1 || world_rank == 2 ){
    MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][0]);
    MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][3]);
  }
  else if(world_rank == 3 || world_rank == 4 || world_rank == 5){
    MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][0]);
  }
  else if (world_rank == 6) {
    MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][2]);
  }
  else if(world_rank == 8){
    MPI_Send(args, n, MPI_DOUBLE, 1, TAG, comms[world_rank][3]);
  }
}

void grid_loop(){
  double **args;
  int n = (maxsize * maxsize) + 4;
  aloc2Dmatrix(&args, 4, n);
  for (int i = 0; i < 4; i++) {
    r[i] = MPI_REQUEST_NULL;
  }
  while(run){
    for (int i = 0; i < 4; i++){
      if(is_valid_connection(i)){
        if(r[i] == MPI_REQUEST_NULL){
          MPI_Comm_rank(comms[world_rank][i], &grank);
          if(grank == 0){
            MPI_Irecv(args[i], n, MPI_DOUBLE, 1, TAG, comms[world_rank][i], &r[i]);
          }
          else{
            MPI_Irecv(args[i], n, MPI_DOUBLE, 0, TAG, comms[world_rank][i], &r[i]);
          }
        }
      }
    }
    int completereqs = 0;
    while (completereqs == 0) {
      for (int i = 0; i < 4; i++) {
        if(is_valid_connection(i)){
          MPI_Test(&r[i], &flag[i], &rstatus);
          if(flag[i]){
            reqorder[completereqs] = i;
            completereqs++;
          }
        }
      }
    }
    for(int i = 0; i < completereqs; i++){
      int j = reqorder[i];
      //CAMINHO DE IDA
      if(args[j][0] == 0){
        grid_loop_go(args[j], j, n);
      }
      else if(args[j][0] == 1){
        grid_loop_back(args[j], j, n);
      }
      else if(args[j][0] == 2){
        grid_loop_kill(args[j], j, n);
        run = 0;
      }
    }//endfor
    sleep(2);
  }//endwhilerun
}

/*MAIN*/

int main(int argc, char **argv){

  /*Initialize MPI*/
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  create_groups();

  init();


  int nreqs = atoi(argv[1]);
  maxsize = atoi(argv[2]);
  int *reqs = malloc(nreqs * sizeof *reqs);
  for(int i = 0; i < nreqs; i++) {
    reqs[i] = atoi(argv[i + 3]);
  }

  /*REQUEST NODE*/
  if(world_rank == 9){
    request_loop(reqs, nreqs, maxsize);
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
