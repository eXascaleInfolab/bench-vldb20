#!/usr/bin/python
import numpy as np
import time

def meanimpute_rmv(filename_input, filename_output, runtime):
    # read input matrix
    matrix = np.loadtxt(filename_input);
    
    # beginning of imputation process - start time measurement
    start_time = time.time();
    
    meanval = [0.0] * len(matrix);
    
    for j in range(0, len(matrix[0])):
        count = 0;
        for i in range(0, len(matrix)):
            if not np.isnan(matrix[i][j]):
                meanval[j] += matrix[i][j];
                count += 1;
            #end if
        #end for
        meanval[j] /= count;
    #end for
    
    for j in range(0, len(matrix[0])):
        for i in range(0, len(matrix)):
            if np.isnan(matrix[i][j]):
                matrix[i][j] = meanval[j];
            #end if
        #end for
    #end for
    
    # imputation is complete - stop time measurement
    end_time = time.time()
    
    # calculate the time elapsed in [!] microseconds
    exec_time = (end_time - start_time) * 1000 * 1000
    
    print "Time (MeanImpute):", exec_time
    
    # we choose a binary flag to indicate whether we need runtime results or algorithm output
    if runtime > 0:
        # if we need runtime we only save one value with the time in microseconds
        np.savetxt(filename_output, np.array([exec_time]))
    else:
        # if we need the output, we instead save the matrix with imputed values to the same file
        np.savetxt(filename_output, matrix)
    # end if
# end function
