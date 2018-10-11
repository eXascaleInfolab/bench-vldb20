set terminal png size 15*{mbsize},720
set output "recovery/figs/{code}_m{len}.png"

set xrange [{mbstart}:{mbstart}+{mbsize}]
set xtics 0,25
#set log y

set key above width -1 vertical maxrows 2 
set tmargin 2.5

set arrow from 0,0 to {nlimit},0 nohead

set xlabel "data point"
set ylabel "value" offset 1.5 

plot\
	'data/reference.txt' index 0 using 1:2 title 'real' with linespoints lt 1 dt 3 lw 3 pt 5 lc rgbcolor "black" pointsize 1, \
	{allplots}

