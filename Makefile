# CC = /usr/local/bin/gcc-6
CC = gcc
MCC = mpicc
CFLAGS = -std=gnu99 -O3 -lm
DATASET = -DCUSTOM_DATASET
PROGRAM = lu
BARRIER = pthread_barrier.c
# BARRIER = ""

compile:
	$(CC) $(CFLAGS) -I utilities utilities/polybench.c util.c $(PROGRAM).c $(DATASET) -o $(PROGRAM).out
	$(CC) $(CFLAGS) -pthread -I utilities utilities/polybench.c util.c $(BARRIER) $(PROGRAM)_pthread.c $(DATASET) -o $(PROGRAM)_pthread.out
	$(MCC) $(CFLAGS) -I utilities utilities/polybench.c util.c $(PROGRAM)_mpi.c $(DATASET) -o $(PROGRAM)_mpi.out

clean:
	rm *.out
