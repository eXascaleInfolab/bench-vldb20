set terminal postscript eps enhanced color "Helvetica" 30
set output "error/figs/{code}_mse.eps"

set xrange [{caseStart}-1:{caseEnd}+1]
set xtics {caseStart},{caseTick}
set log y

set key above width -1 vertical maxrows 2 
set tmargin 2.5

set xlabel "{descr}"
set ylabel "mean squared error" offset 1.5 

plot\
	{mse}


set output "error/figs/{code}_rmse.eps"
set ylabel "root mean squared error" offset 1.5 

plot\
	{rmse}

set output "error/figs/{code}_mae.eps"
set ylabel "mean absolute error" offset 1.5 

plot\
	{mae}
