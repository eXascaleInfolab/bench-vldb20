#!/usr/bin/python
import numpy as np

def meanimpute_recovery(matrix):
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

# end function
