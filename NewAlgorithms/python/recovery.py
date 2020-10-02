#!/usr/bin/python
import numpy as np
import time

def recover_matrix(matrix):
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
    
    return matrix;

#end function


def rmv_main(algcode, filename_input, filename_output, runtime):
    # read input matrix
    matrix = np.loadtxt(filename_input);
    
    # beginning of imputation process - start time measurement
    start_time = time.time();
    
    matrix_imputed = recover_matrix(matrix);
    
    # imputation is complete - stop time measurement
    end_time = time.time()
    
    # calculate the time elapsed in [!] microseconds
    exec_time = (end_time - start_time) * 1000 * 1000
    
    # verification
    nan_mask = np.isnan(matrix_imputed);
    matrix_imputed[nan_mask] = np.sqrt(np.finfo('d').max / 100000.0);
    
    print "Time", algcode, ":", exec_time
    
    # we use a binary flag to indicate whether we need runtime results or algorithm output
    if runtime > 0:
        # if we need runtime, we only save one value with the time in microseconds
        np.savetxt(filename_output, np.array([exec_time]))
    else:
        # if we need the output, we instead save the matrix with imputed values to the same file
        np.savetxt(filename_output, matrix_imputed)
    # end if

# end function
