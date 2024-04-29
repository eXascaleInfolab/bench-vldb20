import pandas as pd
import torch
from utils.regressor import MLPNet
import torch.optim as optim
import copy
import numpy as np
import random
import time
from utils.DynamicGNN import DynamicGCN, DynamicGAT, DynamicGraphSAGE, StaticGCN, StaticGraphSAGE, StaticGAT
from argparse import ArgumentParser
from torch_geometric.nn import knn_graph
from utils.load_dataset import get_model_size
from pypots.utils.metrics import calc_mae, calc_mse, calc_mre


# x = F.relu(self.gc1(x, adj))
# x = F.dropout(x, self.dropout, training=self.training)

parser = ArgumentParser()

parser.add_argument("--input", type=str)
parser.add_argument("--output", type=str)
parser.add_argument("--runtime", type=int, default=0)

parser.add_argument("--incre_mode", type=str, default='alone')

parser.add_argument("--window", type=int, default=2)
parser.add_argument('--stream', type=float, default=1)

parser.add_argument("--eval_ratio", type=float, default=0.05)
parser.add_argument("--epochs", type=int, default=200)
parser.add_argument("--site", type=str, default='KDM')
parser.add_argument("--floor", type=str, default='F1')

parser.add_argument('--base', type=str, default='SAGE')
parser.add_argument("--prefix", type=str, default='testN')

parser.add_argument('--state', type=str, default='true')
parser.add_argument('--thre', type=float, default=0.25)

parser.add_argument('--method', type=str, default='DMU')

parser.add_argument("--num_of_iter", type=int, default=5)
parser.add_argument("--out_channels", type=int, default=256)
parser.add_argument("--k", type=int, default=10)

parser.add_argument("--lr", type=float, default=0.01)
parser.add_argument("--weight_decay", type=float, default=0.1)
parser.add_argument("--dynamic", type=str, default='false')


args = parser.parse_args()

def data_transform(X, X_mask, eval_ratio=0.05):
    eval_mask = np.zeros(X_mask.shape)
    rows, cols = np.where(X_mask==1)
    eval_row_index_index = random.sample(range(len(rows)),int(eval_ratio*len(rows)))
    eval_row_index = rows[eval_row_index_index]
    eval_col_index = cols[eval_row_index_index]
    X_mask[eval_row_index, eval_col_index] = 0
    eval_mask[eval_row_index, eval_col_index] = 1
    eval_X = copy.copy(X)
    X[eval_row_index, eval_col_index] = 0
    return X, X_mask, eval_X, eval_mask


def build_GNN(in_channels, out_channels, k, base):
    if base == 'GAT':
        gnn = DynamicGAT(in_channels=in_channels, out_channels=out_channels, k=k).to(device)
    elif base == 'GCN':
        gnn = DynamicGCN(in_channels=in_channels, out_channels=out_channels, k=k).to(device)
    elif base == 'SAGE':
        gnn = DynamicGraphSAGE(in_channels=in_channels, out_channels=out_channels, k=k).to(device)
    return gnn


def build_GNN_static(in_channels, out_channels, k, base):
    if base == 'GAT':
        gnn = StaticGAT(in_channels=in_channels, out_channels=out_channels, k=k).to(device)
    elif base == 'GCN':
        gnn = StaticGCN(in_channels=in_channels, out_channels=out_channels, k=k).to(device)
    elif base == 'SAGE':
        gnn = StaticGraphSAGE(in_channels=in_channels, out_channels=out_channels, k=k).to(device)
    return gnn


def get_window_data(start, end, ratio):
    X = base_X[int(len(base_X)*start*ratio):int(len(base_X)*end*ratio)]
    X_mask = base_X_mask[int(len(base_X)*start*ratio):int(len(base_X)*end*ratio)]
    return X, X_mask


def window_imputation(start, end, sample_ratio, initial_state_dict=None, X_last=None, mask_last=None, mae_last=None, transfer=False, state=args.state):

    X, X_mask = get_window_data(start=start, end=end, ratio=sample_ratio)

    ori_X = copy.copy(X)
    feature_dim = ori_X.shape[1]
    ori_X_mask = copy.copy(X_mask)

    all_mask = copy.copy(X_mask)
    all_X = copy.copy(X)

    if X_last:
        X_last = np.array(X_last)
        all_X = np.concatenate([X_last, X], axis=0)
        all_mask = np.concatenate([mask_last, X_mask], axis=0)

        X_last = X_last.tolist()

    all_mask_ts = torch.FloatTensor(all_mask).to(device)


    gram_matrix = torch.mm(all_mask_ts, all_mask_ts.t())  # compute the gram product

    gram_vec = gram_matrix.diagonal()

    gram_row_sum = gram_matrix.sum(dim=0)

    value_vec = gram_vec - (gram_row_sum - gram_vec)/(gram_matrix.shape[0]-1)

    keep_index = torch.where(value_vec > args.thre * (feature_dim-1))[0]
    keep_index = keep_index.data.cpu().numpy()

    keep_mask = all_mask[keep_index]

    keep_X = all_X[keep_index]
   
    X, X_mask, eval_X, eval_mask = data_transform(X, X_mask, eval_ratio=args.eval_ratio)

    if X_last:

        X_last = np.array(X_last)
        shp_last = X_last.shape
        eval_X = np.concatenate([X_last, eval_X], axis=0)
        X = np.concatenate([X_last, X], axis=0)
        eval_mask_last = np.zeros(shp_last)
        eval_mask = np.concatenate([eval_mask_last, eval_mask],axis=0)
        X_mask = np.concatenate([mask_last, X_mask], axis=0)

    in_channels = X.shape[1]
    X = torch.FloatTensor(X).to(device)
    X_mask = torch.LongTensor(X_mask).to(device)
    eval_X = torch.FloatTensor(eval_X).to(device)
    eval_mask = torch.LongTensor(eval_mask).to(device)

    # build model
    if args.dynamic == 'true':
        gnn = build_GNN(in_channels=in_channels, out_channels=out_channels, k=args.k, base=args.base)
        gnn2 = build_GNN(in_channels=in_channels, out_channels=out_channels, k=args.k, base=args.base)
    else:
        gnn = build_GNN_static(in_channels=in_channels, out_channels=out_channels, k=args.k, base=args.base)
        gnn2 = build_GNN_static(in_channels=in_channels, out_channels=out_channels, k=args.k, base=args.base)

    model_list = [gnn, gnn2]
    regressor = MLPNet(out_channels, in_channels).to(device)

    if initial_state_dict != None:
        gnn.load_state_dict(initial_state_dict['gnn'])
        gnn2.load_state_dict(initial_state_dict['gnn2'])
        if not transfer:
            regressor.load_state_dict(initial_state_dict['regressor'])

    trainable_parameters = []
    for model in model_list:
        trainable_parameters.extend(list(model.parameters()))

    trainable_parameters.extend(list(regressor.parameters()))
    filter_fn = list(filter(lambda p: p.requires_grad, trainable_parameters))

    num_of_params = sum(p.numel() for p in filter_fn)

    model_size = get_model_size(gnn) + get_model_size(gnn2) + get_model_size(regressor)
    model_size = round(model_size, 6)

    num_of_params = num_of_params/1e6

    opt = optim.Adam(filter_fn, lr=lr, weight_decay=weight_decay)

    graph_impute_layers = len(model_list)


    X_knn = copy.deepcopy(X)

    edge_index = knn_graph(X_knn, args.k, batch=None, loop=False, cosine=False)

    min_mae_error = 1e9
    min_mse_error = None
    min_mape_error = None
    opt_epoch = None
    opt_time = None
    best_X_imputed = None
    best_state_dict = None

    for pre_epoch in range(epochs):
        gnn.train()
        gnn2.train()
        regressor.train()
        opt.zero_grad()
        loss = 0
        X_imputed = copy.copy(X)

        for i in range(graph_impute_layers):
            if args.dynamic == 'true':
                X_emb = model_list[i](X_imputed)
            else:
                X_emb, edge_index = model_list[i](X_imputed, edge_index)

            pred = regressor(X_emb)
            X_imputed = X*X_mask + pred*(1 - X_mask)
            temp_loss = torch.sum(torch.abs(X - pred) * X_mask) / (torch.sum(X_mask) + 1e-5)
            # print('temp loss:', temp_loss.item())
            loss += temp_loss

        loss.backward()
        opt.step()
        train_loss = loss.item()
        print('{n} epoch loss:'.format(n=pre_epoch), train_loss)

        trans_X = copy.copy(X_imputed)
        trans_eval_X = copy.copy(eval_X)

        epoch_state_dict = {'gnn': gnn.state_dict(), 'gnn2': gnn2.state_dict(),  'regressor': regressor.state_dict()}

        gnn.eval()
        gnn2.eval()
        regressor.eval()

        with torch.no_grad():
            mae_error = calc_mae(trans_X, trans_eval_X, eval_mask)

            mse_error = calc_mse(trans_X, trans_eval_X, eval_mask)  # calculate mean absolute error on the ground truth (artificially-missing values)

            mape_error = calc_mre(trans_X, trans_eval_X, eval_mask)

            print('valid impute error MAE:', mae_error.item())
            print('valid impute error MSE:', mse_error.item())
            print('valid impute error MRE:', mape_error.item())

            if mae_error.item() < min_mae_error:
                opt_epoch = copy.copy(pre_epoch)
                min_mae_error = round(mae_error.item(), 6)
                print('{epoch}_opt_mae_error'.format(epoch=pre_epoch), min_mae_error)

                min_mse_error = round(mse_error.item(), 6)
                min_mape_error = round(mape_error.item(), 6)

                print('{epoch}_opt time:'.format(epoch=pre_epoch), opt_time)

                best_X_imputed = copy.copy(X_imputed)
                best_X_imputed = best_X_imputed*(1-ori_X_mask) + ori_X*ori_X_mask
                best_state_dict = copy.copy(epoch_state_dict)


    results_list = [opt_epoch, min_mae_error, min_mse_error, min_mape_error, num_of_params, model_size, opt_time, 0]

    if mae_last and (min_mae_error > mae_last) and (state == 'true'):
        best_state_dict = copy.copy(initial_state_dict)
    return best_state_dict, keep_X.tolist(), keep_mask, results_list, min_mae_error, best_X_imputed

 
if __name__ == "__main__":

    INPUT = args.input
    OUTPUT = args.output
    rt = args.runtime

    start = time.time()
    torch.random.manual_seed(0)
    device = torch.device('cpu')
    out_channels = args.out_channels
    lr = args.lr
    weight_decay = args.weight_decay
    epochs = args.epochs

    print('state', args.state)
    print('thre', args.thre)
    random.seed(0)
    base_X = np.loadtxt(INPUT)
    base_X_mask = (~np.isnan(base_X)).astype(int)
    base_X = np.nan_to_num(base_X)
    mean_X = np.mean(base_X)
    std_X = np.std(base_X)
    # base_X = (base_X - mean_X) / std_X



    incre_mode = args.incre_mode # 'alone',  'data', 'state', 'state+transfer', 'data+state', 'data+state+transfer'
    prefix = args.prefix
 
    num_windows = 1

    results_schema = ['opt_epoch', 'opt_mae', 'mse', 'mape', 'para', 'memo', 'opt_time', 'tot_time']

    num_of_iteration = args.num_of_iter
    iter_results_list = []
    best_X_imputed=0

    for iteration in range(num_of_iteration):
        results_collect = []
        for w in range(num_windows):
            print(f'which time window:{w}')
            if w == 0 :
                window_best_state, X_last, mask_last, window_results, mae_last, best_X = window_imputation(start=w, end=w+1, sample_ratio=1/num_windows)
                results_collect.append(window_results)
                best_X_imputed = best_X
            else:
                if incre_mode == 'alone':
                    window_best_state, X_last, mask_last, window_results, mae_last, best_X = window_imputation(start=w, end=w+1, sample_ratio=1/num_windows)
                    results_collect.append(window_results)
                    best_X_imputed = best_X
                    
                elif incre_mode == 'data':
                    window_best_state, X_last, mask_last, window_results, mae_last, best_X = window_imputation(start=w, end=w + 1, sample_ratio=1/num_windows, X_last=X_last, mask_last=mask_last)
                    results_collect.append(window_results)
                    best_X_imputed = best_X

                elif incre_mode == 'state':
                    window_best_state, X_last, mask_last, window_results, mae_last, best_X = window_imputation(start=w, end=w + 1, sample_ratio=1/num_windows,initial_state_dict=window_best_state, mae_last=mae_last)
                    results_collect.append(window_results)
                    best_X_imputed = best_X

                elif incre_mode == 'state+transfer':
                    window_best_state, X_last, mask_last, window_results, mae_last, best_X = window_imputation(start=w, end=w + 1, sample_ratio=1/num_windows,initial_state_dict=window_best_state, transfer=True, mae_last=mae_last)
                    results_collect.append(window_results)
                    best_X_imputed = best_X

                elif incre_mode == 'data+state':
                    window_best_state, X_last, mask_last, window_results, mae_last, best_X = window_imputation(start=w, end=w+1, sample_ratio=1/num_windows, initial_state_dict=window_best_state, X_last=X_last, mask_last=mask_last, mae_last=mae_last)
                    results_collect.append(window_results)
                    best_X_imputed = best_X

                elif incre_mode == 'data+state+transfer':
                    window_best_state, X_last, mask_last, window_results, mae_last, best_X = window_imputation(start=w, end=w+1, sample_ratio=1/num_windows, initial_state_dict=window_best_state, X_last=X_last, mask_last=mask_last, transfer=True, mae_last=mae_last)
                    results_collect.append(window_results)
                    best_X_imputed = best_X

        df = pd.DataFrame(results_collect, index=range(num_windows), columns=results_schema)
        iter_results_list.append(df)
        # print(res.shape)
    print('done!')
    end = time.time()
    runtime = (end-start)*1000*1000

    print(f"Total runtime = {runtime/1e6} s")

    # print('ready to write data!')
    if rt == 0:
        avg_df = sum(iter_results_list)/num_of_iteration
        # avg_df = pd.DataFrame(avg_df.mean()).T
        np.savetxt(OUTPUT, np.array(best_X_imputed),delimiter=',', fmt='%f')
        avg_df = avg_df.round(4)
    else:
        np.savetxt(OUTPUT, np.array([runtime]))
 
