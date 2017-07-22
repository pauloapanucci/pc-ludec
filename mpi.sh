# mpirun -n 2 -N 1  -hostfile ips.txt ./lu_mpi.out
mpirun -n 13 ./lu_grid.out

#! /bin/bash
PROGRAM="lu"

echo "LU MPI"

# SEQUENTIAL LU
# echo "SEQUENTIAL"
file="./data/${PROGRAM}_mpi.data"
if [ -f "$file" ]
then
	rm $file
fi
mpirun -n 2 -N 1 -hostfile 2ips.txt ./${PROGRAM}_mpi.out >> ./data/${PROGRAM}_mpi.data 2>&1
mpirun -n 4 -N 1 -hostfile 4ips.txt ./${PROGRAM}_mpi.out >> ./data/${PROGRAM}_mpi.data 2>&1
mpirun -n 8 -N 1 -hostfile 8ips.txt ./${PROGRAM}_mpi.out >> ./data/${PROGRAM}_mpi.data 2>&1
mpirun -n 16 -N 1 -hostfile 16ips.txt ./${PROGRAM}_mpi.out >> ./data/${PROGRAM}_mpi.data 2>&1
