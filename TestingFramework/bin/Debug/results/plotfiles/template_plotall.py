#! /usr/bin/python
import subprocess
import time

# technical
def my_range(start, end, step):
    while start <= end:
        yield start
        start += step

GNUPLOTexec = 'gnuplot';
Rexec = 'Rscript';

def launchGP(arg):
    s = subprocess.check_output([GNUPLOTexec, arg]);
    time.sleep(0.05);


def launchR(arg):
    s = subprocess.check_output([Rexec, arg]);
    time.sleep(0.25);


# plots
mainFolder = "recovery/scripts/"
errFolder = "error/results/"

# ref+mse
launchR(errFolder + "error_calculation.r");
launchGP(errFolder + "{code}_mse.plt");

launchGP(mainFolder + "reference_plot.plt");

#testcase
for tcase in my_range({start}, {end}, {tick}):
    plotFile = mainFolder + "{code}_m" + str(tcase) + ".plt";
    launchGP(plotFile);


