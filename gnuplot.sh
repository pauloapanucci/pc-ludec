#! /usr/local/bin/gnuplot
# set terminal svg enhanced font "Helvetica,20"
set terminal png size 800,600 enhanced font "Helvetica,12"
# set term x11 persist
# set xlabel "Threads"
# set ylabel "Speedup"
# set xtics 1
# set ytics 1
# set key left top
# f(x)=x
#
# set title 'Cholesky Speedup Compared to Polybench Sequential'
# set output "polybench.png"
# plot f(x) dashtype 30 title 'Ideal', "./data/polybench_compare.data" using 1:2 title 'OpenMP' with lines, "./data/polybench_compare.data" using 1:3 title 'Pthread' with lines
# set title 'Cholesky Speedup Compared to Developed Sequential'
# set output "developed.png"
# plot f(x) dashtype 30 title 'Ideal', "./data/developed_compare.data" using 1:2 title 'OpenMP' with lines, "./data/developed_compare.data" using 1:3 title 'Pthread' with lines

red = "#FF0000"; green = "#00FF00"; blue = "#0000FF"; skyblue = "#87CEEB";
# set yrange [0:20]
set style data histogram
set style histogram cluster gap 1
set style fill solid
set boxwidth 0.9
set xtics format ""
set grid ytics
set output "cache.png"
set title "Cache Miss"
plot "./papidata/cacheplot.data" using 2:xtic(1) title "L1" linecolor rgb red, \
            '' using 3 title "L2" linecolor rgb blue, \
            '' using 4 title "L3" linecolor rgb green,
