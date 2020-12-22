import json
import numpy as np
from random import randrange

def prepare_dat(input, output):
    delta = 0.0
    masks = 1
    eval_masks = 0
    data = {}
    data['forward'] = []
    firstIteration = True

    file = open(input, "r")

    for v in file:
        value = float(v)
        eval = float(v)
        
        if not np.isnan(value):
            #value is NOT missing
            masks = 1
            eval_masks = 0
        else:
            #value is missing
            masks = 0
            eval_masks = 1
            
            value = 0.0
            eval = 0.0
            if not firstIteration:
                delta = delta + 1.0
        #endif
        
        data['forward'].append({
            'evals': [eval],
            'deltas': [delta],
            'forwards': [0.0],
            'masks': [masks],
            'values': [value],
            'eval_masks': [eval_masks],
        })
        
        firstIteration = False
    #end for

    data['backward'] = []

    firstIteration = True
    delta = 0.0
    for e in reversed(data['forward']):
        newDict = e.copy()
        if not firstIteration and newDict['masks'][0] == 0:
            delta = delta - 1.0
        newDict['deltas'] = [delta]
        data['backward'].append(newDict)
        
        firstIteration = False
    #end for

    data['label'] = 0

    with open(output, 'w') as outfile:
        json.dump(data, outfile)
#end function
