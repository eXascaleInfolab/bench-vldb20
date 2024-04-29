import numpy as np

def mean_recovery(matrix):
    means = np.nanmean(matrix,axis=0)
    for i in range(matrix.shape[1]):
        matrix[:,i] = np.nan_to_num(matrix[:,i],nan=means[i])
    return matrix


def zero_recovery(matrix):
    return np.nan_to_num(matrix)
