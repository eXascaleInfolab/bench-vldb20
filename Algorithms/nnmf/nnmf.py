from pandas import DataFrame;
import numpy as np;
from sklearn.decomposition import NMF;
import time;


def nnmf_impute(mx, nrows, ncols, k, missingmask, shift, rtstream):
    # Example data matrix X
    mxmin = 0;

    if shift:
        for i in xrange(nrows):
            for j in xrange(ncols):
                if mx[i][j] < mxmin:
                    mxmin = mx[i][j];
        # end for
        mxmin = abs(mxmin);  # shift size
        #print ("mxmin=" + str(mxmin));

        mx = mx + (mxmin + 1.0); # 1 ~= stddev of data

    X = DataFrame(mx, index=range(nrows), columns=range(ncols));

    # Hiding values to test imputation
    X_imputed = X.copy();
    X_imputed.values[missingmask] = 0;  # init at 0 for recovery to converge in one direction for all elements

    # Initializing model
    start_time = time.time();

    nmf_model = NMF(n_components=k);
    nmf_model.fit(X_imputed.values);

    if rtstream:
        start_time = time.time();

    # iterate model
    err = 1;
    iter = 0;
    while err > 1E-6 and iter < 100:
        iter += 1;
        W = nmf_model.fit_transform(X_imputed.values);
        reconstruction = W.dot(nmf_model.components_);

        # calc diff
        diffvector = reconstruction - X_imputed.values;
        diffvector[~missingmask] = 0;
        diffvector = np.multiply(diffvector, diffvector);

        err = np.sum(np.sum(diffvector));

        diffvector[missingmask] = 1;
        blocksize = np.sum(np.sum(diffvector));

        err = np.sqrt(err / blocksize);

        # dump
        #print "iter= " + str(iter) + "; reconstr_err=" + str(nmf_model.reconstruction_err_) + "; delta=" + str(err);

        # impute
        X_imputed.values[missingmask] = reconstruction[missingmask];
    # end while

    end_time = time.time();
    result = X_imputed.values - (mxmin + 1.0);

    print "iter_cnt= " + str(iter) + "; last_reconstr_err=" + str(nmf_model.reconstruction_err_) + "; last_delta=" + str(err) + "; time=" +str(end_time - start_time);

    return (result, end_time - start_time);

