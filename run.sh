# clear
mpicc util.c lu_grid.c -o lu_grid.out
mpirun -n 13 ./lu_grid.out 3 256 256 128 64
# mpirun -n 13 xterm -e gdb ./grid.out 1
