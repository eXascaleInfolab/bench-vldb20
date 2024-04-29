#!/usr/bin/python
import numpy as np
import time

from transformer import transformer_recovery

def recover_matrix(matrix):
    np.save('org_matrix',matrix)
    temp = transformer_recovery(matrix)
    np.save('recovered_matrix',temp)
    return temp

# end function


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
    
    print ("Time", algcode, ":", exec_time)
    
    # we use a binary flag to indicate whether we need runtime results or algorithm output
    if runtime > 0:
        # if we need runtime, we only save one value with the time in microseconds
        np.savetxt(filename_output, np.array([exec_time]))
    else:
        # if we need the output, we instead save the matrix with imputed values to the same file
        np.savetxt(filename_output, matrix_imputed)
    # end if

# end function

if __name__ == '__main__' :
    #temp = np.zeros((1000,100))
    temp = np.load('org_matrix.npy')
    recover_matrix(temp)
