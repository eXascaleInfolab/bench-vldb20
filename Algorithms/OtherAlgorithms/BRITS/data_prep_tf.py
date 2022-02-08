import json
import numpy as np
import pandas as pd
from random import randrange

def parse_delta(masks, dir_):
    if dir_ == "backward":
        masks = masks[::-1]

    deltas = []

    for t in range(len(masks)):
        if t == 0:
            deltas.append(np.ones(1))
        else:
            # if the value is not missing delta is 1 otherwise is 1 + prev delta
            deltas.append(np.ones(1) + (1 - masks[t]) * deltas[-1])

    return np.array(deltas)


def parse_rec(values, masks, evals, eval_masks, dir_):
    deltas = parse_delta(masks, dir_)

    forwards = pd.DataFrame(values).fillna(method="ffill").fillna(0.0).as_matrix()

    rec = {}

    rec["values"] = np.nan_to_num(values).tolist()
    rec["masks"] = masks.astype("int32").tolist()
    # imputation ground-truth
    rec["evals"] = np.nan_to_num(evals).tolist()
    rec["eval_masks"] = eval_masks.astype("int32").tolist()
    rec["forwards"] = forwards.tolist()
    rec["deltas"] = deltas.tolist()

    return rec


def prepare_dat(input, output):

    file = open(output, 'w')
    src = pd.read_csv(input, header=None, delimiter=" ")

    for col in range(src.shape[1]):

        evals = []
        values = []

        for v in src[col].tolist():
        
            evals.append([float(v)])

        evals = np.array(evals)
        shp = evals.shape
        evals = evals.reshape(-1)
        values = evals.copy()

        masks = ~np.isnan(values)
        eval_masks = (~np.isnan(values)) ^ (~np.isnan(evals))

        evals = evals.reshape(shp)
        values = values.reshape(shp)
        masks = masks.reshape(shp)
        eval_masks = eval_masks.reshape(shp)

        rec = {"label": 0}

        rec["forward"] = parse_rec(values, masks, evals, eval_masks, dir_="forward")
        rec["backward"] = parse_rec(values[::-1], masks[::-1], evals[::-1], eval_masks[::-1], dir_="backward")

        rec = json.dumps(rec)
        file.write(rec + "\n")
        
#end function
