# CC = /usr/local/bin/gcc-6
CC = gcc
MCC = mpicc
CFLAGS = -std=gnu99 -O3
DATASET = -DLARGE_DATASET
PROGRAM = lu
# BARRIER = pthread_barrier.c
# BARRIER = ""

all: sequential pthread mpi grid

sequential:
	$(CC) $(CFLAGS) util.c $(PROGRAM).c $(DATASET) -o $(PROGRAM).out -lm

pthread:
	$(CC) $(CFLAGS) -pthread util.c $(PROGRAM)_pthread.c $(DATASET) -o $(PROGRAM)_pthread.out -lm

mpi:
	$(MCC) $(CFLAGS) util.c $(PROGRAM)_mpi.c $(DATASET) -o $(PROGRAM)_mpi.out

grid:
	$(MCC) $(CFLAGS) util.c $(PROGRAM)_grid.c $(DATASET) -o $(PROGRAM)_grid.out

clean:
	rm *.out
