set terminal png size 10*{nlimit},720

set output "recovery/figs/reference_full.png"
set xrange [1:{nlimit}]

set xtics 0,100

set key above width -1 vertical maxrows 2 
set tmargin 2.5

set xlabel "data point"
set ylabel "value" offset 1.5 

set arrow from 0,0 to {nlimit},0 nohead

plot\
	'data/reference.txt' index 0 using 1:2 title 't1' with linespoints 	lt 1  lw 3 lc rgbcolor "cyan" pointsize 1 pt 3, \
	'data/reference.txt' index 0 using 1:3 title 't2' with linespoints 	lt 2 lw 3 lc rgbcolor "violet" pointsize 1 pt 6, \
	'data/reference.txt' index 0 using 1:4 title 't3' with linespoints 	lt 3 lw 3 lc rgbcolor "red" pointsize 1 pt 2, \
	'data/reference.txt' index 0 using 1:5 title 't4' with linespoints lt 2 lw 3 lc rgbcolor "black" pointsize 1 pt 3


