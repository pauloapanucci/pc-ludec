# CC = /usr/local/bin/gcc-6
CC = gcc
MCC = mpicc
CFLAGS = -std=gnu99 -O3 -lm
DATASET = -DMEDIUM_DATASET
PROGRAM = lu
BARRIER = pthread_barrier.c
# BARRIER = ""

compile:
	# $(CC) $(CFLAGS) -I utilities utilities/polybench.c util.c $(PROGRAM).c $(DATASET) -o $(PROGRAM).out
	# $(CC) $(CFLAGS) -pthread -I utilities utilities/polybench.c util.c $(BARRIER) $(PROGRAM)_pthread.c $(DATASET) -o $(PROGRAM)_pthread.out
	$(MCC) $(CFLAGS) util.c $(PROGRAM)_mpi.c $(DATASET) -o $(PROGRAM)_mpi.out
	# $(MCC) $(CFLAGS) util.c $(PROGRAM)_grid.c $(DATASET) -o $(PROGRAM)_grid.out
clean:
	rm *.out
