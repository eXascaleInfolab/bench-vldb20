########################################
## code copied and adapted from:      ##
## https://github.com/lmluzern/BRITS/ ##
########################################

import copy
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.optim.lr_scheduler import StepLR

import numpy as np

import time
import utils
import models
import argparse
import data_loader
import pandas as pd
import ujson as json

from sklearn import metrics

from ipdb import set_trace

from data_prep_tf import prepare_dat

parser = argparse.ArgumentParser()
parser.add_argument('--epochs', type = int, default = 100)
parser.add_argument('--batch_size', type = int, default = 64)
parser.add_argument('--model', type = str)
parser.add_argument('--input', type = str)
parser.add_argument('--output', type = str)
parser.add_argument('--runtime', type = int, default = 0)
args = parser.parse_args()

def train(model, input):
    optimizer = optim.Adam(model.parameters(), lr = 1e-3)
    data_iter = data_loader.get_loader(input, batch_size = args.batch_size)

    for epoch in xrange(args.epochs):
        model.train()

        run_loss = 0.0

        for idx, data in enumerate(data_iter):
            data = utils.to_var(data)
            ret = model.run_on_batch(data, optimizer)

            run_loss += ret['loss'].data

            print '\r Progress epoch {}, {:.2f}%, average loss {}'.format(epoch, (idx + 1) * 100.0 / len(data_iter), run_loss / (idx + 1.0)),
        #end for
    #end for    
    
    return (model, data_iter)
#end function

def evaluate(model, val_iter):
    model.eval()

    #evals = []
    imputations = []

    for idx, data in enumerate(val_iter):
        data = utils.to_var(data)
        ret = model.run_on_batch(data, None)
        
        imputation = ret['imputations'].data.cpu().numpy()
        imputations += imputation.tolist()
    #end for

    #evals = np.asarray(evals)
    imputations = np.asarray(imputations)
    return imputations
#end function

def run(input, output, rt = 0):
    matrix = np.loadtxt(input)
    n = len(matrix)
    prepare_dat(input, input + ".tmp")

    start = time.time()
    model = getattr(models, args.model).Model(n)

    if torch.cuda.is_available():
        model = model.cuda()

    (model, data_iter) = train(model, input + ".tmp")
    res = evaluate(model, data_iter)
    end = time.time()

    if rt > 0:
        np.savetxt(output, np.array([(end - start) * 1000 * 1000]))
    else:
        res = res[0,:n]
        res = res.reshape(n)
        matrix[:, 0] = res
        np.savetxt(output, matrix)
    #end if

    print ''
    print 'Time (BRITS):', ((end - start) * 1000 * 1000)
#end function

if __name__ == '__main__':
    input = args.input
    output = args.output
    run(input, output, args.runtime)
