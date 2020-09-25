#############################################################
#
# Robust Synthetic Control Tests (based on ALS)
#
# You need to ensure that this script is called from
# the tslib/ parent directory or tslib/tests/ directory:
#
# 1. python tests/testScriptSynthControlALS.py
# 2. python testScriptSynthControlALS.py
#
#############################################################
import sys, os

import numpy as np
import pandas as pd
import copy
import math
import time

sys.path.append("..")
sys.path.append(os.getcwd())

from src import tsUtils
from src.synthcontrol.syntheticControl import RobustSyntheticControl
from tests import testdata


def rmv_mydata(filename, matrix_in, matrix_out, runtime, k):
    df = pd.read_csv(filename)
    pivot = df.pivot_table(values='val', index='ts', columns='time')
    df_rec = pd.DataFrame(pivot.to_records())

    time_series = list(np.unique(df_rec['ts']))

    ts0key = 'TS0'
    time_series.remove(ts0key)
    other_series = time_series

    idx_start = 0
    idx_train_end = 0
    idx_test_end = df_rec[df_rec['ts'] == ts0key].size - 1

    for i in range(idx_test_end):
        elem = df_rec[df_rec['ts'] == ts0key][str(i)].values[0]
        if math.isnan(elem):
            idx_train_end = i
            break

    training_idxs = []
    for i in range(idx_start, idx_train_end, 1):
        training_idxs.append(str(i))

    test_idxs = []
    for i in range(idx_train_end, idx_test_end, 1):
        test_idxs.append(str(i))

    train_data_master_dict = {}
    train_data_dict = {}
    test_data_dict = {}
    for key in other_series:
        series = df_rec[df_rec['ts'] == key]

        train_data_master_dict.update({key: series[training_idxs].values[0]})

        # randomly hide training data
        (trainData, pObservation) = tsUtils.randomlyHideValues(copy.deepcopy(train_data_master_dict[key]), 1.0)
        train_data_dict.update({key: trainData})
        test_data_dict.update({key: series[test_idxs].values[0]})

    series = df_rec[df_rec['ts'] == ts0key]
    train_data_master_dict.update({ts0key: series[training_idxs].values[0]})
    train_data_dict.update({ts0key: series[training_idxs].values[0]})
    test_data_dict.update({ts0key: series[test_idxs].values[0]})

    train_master_df = pd.DataFrame(data=train_data_master_dict)
    train_df = pd.DataFrame(data=train_data_dict)
    test_df = pd.DataFrame(data=test_data_dict)

    start_time = time.time()

    # model
    rscModel = RobustSyntheticControl(ts0key, k, len(train_df), probObservation=1.0, modelType='svd',
                                      otherSeriesKeysArray=other_series)

    # fit the model
    rscModel.fit(train_df)

    # save the denoised training data
    denoisedDF = rscModel.model.denoisedDF()

    # predict - all at once
    predictions = rscModel.predict(test_df)

    end_time = time.time()
    timev = end_time - start_time

    mat = np.loadtxt(matrix_in)

    mat = np.array(mat)

    for i in range(idx_train_end, idx_test_end, 1):
        mat[i] = predictions[i - idx_train_end]

    print "Time (SSA):", (timev * 1000 * 1000)

    if runtime > 0:
        np.savetxt(matrix_out, np.array([timev * 1000 * 1000]))  # to microsec
    else:
        np.savetxt(matrix_out, mat)
    # end if

# end function


def main():
    print("*******************************************************")
    print("*******************************************************")
    print("********** Running the Testing Scripts. ***************")

    print("-------------------------------------------------------")
    print("---------- Robust Synthetic Control (ALS). ------------------")
    print("-------------------------------------------------------")

    directory = os.path.dirname(testdata.__file__)

    print("    Basque Country                  ")
    basqueFilename = directory + '/basque.csv'
    #basque(basqueFilename)
    rmv_mydata(basqueFilename, "data_in/airq_ref_nonum.txt", "data_out/airq_rec.txt", 0, 2)

    print("-------------------------------------------------------")
    print("********** Testing Scripts Done. **********************")
    print("*******************************************************")
    print("*******************************************************")


if __name__ == "__main__":
    main()
