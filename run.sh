clear
mpicc util.c grid.c -o grid.out
mpirun -n 13 ./grid.out 1
# mpirun -n 13 xterm -e gdb ./grid.out 1
