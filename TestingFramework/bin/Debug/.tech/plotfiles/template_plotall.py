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
mainFolder = "scripts/precision/"

# ref+mse
launchR(mainFolder + "error_calculation.r");
launchGP(mainFolder + "{code}_mse.plt");

{vis}launchGP(mainFolder + "reference_plot.plt");

#testcase
for tcase in my_range({start}, {end}, {tick}):
    plotFile = mainFolder + "{code}_m" + str(tcase) + ".plt";
    {vis}launchGP(plotFile);


