# pc-ludec
Decomposição LU Pthread e MPI. Malha de Passagem de Mensagens com Processamento de Fatoração LU.

# Compilando
 - Para compilar o código sequencial basta digitar $ make sequential;
 - Para compilar o código Pthread basta digitar $ make pthread;
 - Para compilar o código MPI basta digitar $ make mpi;
 - Para compilar o código do grid basta digitar $ make grid;
 - Para compilar todos os códigos basta digitar $ make all;
 - Para remover os arquivos compilados basta digitar $ make clean.

# Executando
- Para executar o código sequencial basta digitar $ ./lu.out;
- Para executar o código Pthread basta digitar $ ./lu_pthread.out \<nthreads\>, em que \<nthreads\> é a quantidade de threads passada como argumento.
- Para executar o código MPI para várias máquinas basta digitar $ mpirun -n \<nprocs\> --hostfile \<hostifile\> ./lu_mpi.out, em que \<nprocs\> é a quantidade de processos e \<hostifile\> o arquivo com os ips das máquinas que executarão o código. A máquina host deverá ter o seu ip como o primeiro da lista.
- Para executar o código MPI localmente basta digitar $ mpirun -n \<nprocs\> ./lu_mpi.out, em que \<nprocs\> é a quantidade de processos.
- Para executar o código do grid basta digitar $ mpirun -n 13 ./lu_grid.out \<nreqs\> \<maxsize\> \<req1size, req2size...\>, em que \<nreqs\> é a quantidade de requisições de resolução de fatoração LU a ser feita, \<maxsize\> o tamanho máximo que uma requisição pode ter e \<req1size, req2size...\> os tamanhos de cada requisição. Como exemplo, caso queira 3 requisições, uma de 64, outra de 128 e outra de 256, deve ser executar o seguinte comando: mpirun -n 13 ./lu_grid.out 3 256 64 128 256. O número de processos deve sempre ser 13, pois o algoritmo foi modelado para o modelo de grid contido na pasta de imagens.
