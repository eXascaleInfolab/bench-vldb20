set terminal postscript eps enhanced color "Helvetica" 30
set output "runtime/plots/{code}_rt.eps"

set xrange [{caseStart}:{caseEnd}]
set xtics {caseStart},{caseTick}
#set log y

set key above width -2 vertical maxrows 3
set tmargin 4.0

set xlabel "{descr}"
set ylabel "running time (microseconds)" offset 1.5 

plot\
	{rt}


set output "runtime/plots/{code}_rt_log.eps"
set log y

plot\
	{rt}


