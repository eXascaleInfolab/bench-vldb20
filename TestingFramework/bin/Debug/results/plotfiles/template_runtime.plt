set terminal postscript eps enhanced color "Helvetica" 30
set output "figs/{code}_rt.eps"

set xrange [{caseStart}:{caseEnd}]
set xtics {caseStart},{caseTick}
#set log y

set key above width -1 vertical maxrows 2 
set tmargin 2.5

set xlabel "{descr}"
set ylabel "running time (microseconds)" offset 1.5 

plot\
	{rt}


set output "figs/{code}_rt_log.eps"
set log y

plot\
	{rt}


