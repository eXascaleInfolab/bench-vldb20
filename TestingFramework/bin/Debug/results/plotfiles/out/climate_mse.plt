set terminal postscript eps enhanced color "Helvetica" 30
set output "error/figs/climate_mse.eps"

set xrange [100-1:800+1]
set xtics 100,100
#set log y

set key above width -1 vertical maxrows 2 
set tmargin 2.5

set xlabel "number of missing values"
set ylabel "mean squared error" offset 1.5 

plot\
	'error/results/values/mse/MSE_incd_k3.dat' index 0 using 1:2 title 'incd\_k3' with linespoints lt 8 lw 3 pt 7 lc rgbcolor "red" pointsize 2, \
	'error/results/values/mse/MSE_incd_k2.dat' index 0 using 1:2 title 'incd\_k2' with linespoints lt 8 lw 3 pt 7 lc rgbcolor "dark-red" pointsize 1, \
	'error/results/values/mse/MSE_incd_k1.dat' index 0 using 1:2 title 'incd\_k1' with linespoints lt 8 lw 3 pt 7 lc rgbcolor "red" pointsize 0, \
	'error/results/values/mse/MSE_stmvl.dat' index 0 using 1:2 title 'stmvl' with linespoints lt 8 dt 3 lw 3 pt 5 lc rgbcolor "cyan" pointsize 2, \
	'error/results/values/mse/MSE_nnmf.dat' index 0 using 1:2 title 'nnmf' with linespoints lt 8 dt 5 lw 3 pt 8 lc rgbcolor "blue" pointsize 2, \
	'error/results/values/mse/MSE_grouse.dat' index 0 using 1:2 title 'grouse' with linespoints lt 8 dt 2 lw 3 pt 7 lc rgbcolor "yellow" pointsize 2, \
	'error/results/values/mse/MSE_trmf.dat' index 0 using 1:2 title 'trmf' with linespoints lt 8 dt 2 lw 3 pt 2 lc rgbcolor "dark-violet" pointsize 2, \


set output "error/figs/climate_rmse.eps"
set ylabel "root mean squared error" offset 1.5 

plot\
	'error/results/values/rmse/RMSE_incd_k3.dat' index 0 using 1:2 title 'incd\_k3' with linespoints lt 8 lw 3 pt 7 lc rgbcolor "red" pointsize 2, \
	'error/results/values/rmse/RMSE_incd_k2.dat' index 0 using 1:2 title 'incd\_k2' with linespoints lt 8 lw 3 pt 7 lc rgbcolor "dark-red" pointsize 1, \
	'error/results/values/rmse/RMSE_incd_k1.dat' index 0 using 1:2 title 'incd\_k1' with linespoints lt 8 lw 3 pt 7 lc rgbcolor "red" pointsize 0, \
	'error/results/values/rmse/RMSE_stmvl.dat' index 0 using 1:2 title 'stmvl' with linespoints lt 8 dt 3 lw 3 pt 5 lc rgbcolor "cyan" pointsize 2, \
	'error/results/values/rmse/RMSE_nnmf.dat' index 0 using 1:2 title 'nnmf' with linespoints lt 8 dt 5 lw 3 pt 8 lc rgbcolor "blue" pointsize 2, \
	'error/results/values/rmse/RMSE_grouse.dat' index 0 using 1:2 title 'grouse' with linespoints lt 8 dt 2 lw 3 pt 7 lc rgbcolor "yellow" pointsize 2, \
	'error/results/values/rmse/RMSE_trmf.dat' index 0 using 1:2 title 'trmf' with linespoints lt 8 dt 2 lw 3 pt 2 lc rgbcolor "dark-violet" pointsize 2, \

