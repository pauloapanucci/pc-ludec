#! /usr/local/bin/gnuplot
# set terminal svg enhanced font "Helvetica,20"
set terminal png size 800,600 enhanced font "Helvetica,12"
# set term x11 persist
set xlabel "Threads/Processos"
set ylabel "Speedup"
set xtics 1
set ytics 1
set key left top
f(x)=x

set title 'LU - Speedup de Pthread e MPI'
set output "lu-speedup.png"
plot f(x) dashtype 30 title 'Ideal', "./data/lu_speedup.data" using 1:2 title 'Pthread' with lines, "./data/lu_speedup.data" using 1:3 title 'MPI' with lines
