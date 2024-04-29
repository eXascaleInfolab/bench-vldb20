#!/usr/bin/python
import numpy as np
import argparse
import time

from transformer import transformer_recovery

parser = argparse.ArgumentParser()
parser.add_argument('--input', type = str)
parser.add_argument('--output', type = str)
parser.add_argument('--runtime', type = int, default = 0)
args = parser.parse_args()

def recover_matrix(matrix):
    temp = transformer_recovery(matrix)
    return temp

# end function


def rmv_main(input, output, rt = 0):
    # read input matrix
    matrix = np.loadtxt(input);
    
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
    
    print("Time", "deepMVI", ":", exec_time)
    
    # we use a binary flag to indicate whether we need runtime results or algorithm output
    if rt > 0:
        # if we need runtime, we only save one value with the time in microseconds
        np.savetxt(output, np.array([exec_time]))
    else:
        # if we need the output, we instead save the matrix with imputed values to the same file
        np.savetxt(output, matrix_imputed)
    # end if

# end function

if __name__ == '__main__' :
    input = args.input
    output = args.output
    rmv_main(input, output, args.runtime)
