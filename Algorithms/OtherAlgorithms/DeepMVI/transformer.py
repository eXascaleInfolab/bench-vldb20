import torch
import numpy as np
import random
import copy
import utils
from loader import *
from model import *

interval = 0

def train(model,train_loader,val_loader,device):
    best_state_dict = model.state_dict()
    best_loss = float('inf')

    lr = 1e-3
    optim = torch.optim.Adam(model.parameters(),lr=lr)

    iteration = 0
    start_epoch = 0
    tolerance_epoch = 0
    patience = 2
    max_epoch = 1000
    train_error = 0
    for epoch in range(start_epoch,max_epoch):
        print("Starting Epoch : %d"%epoch)

        for inp_,mask,residuals,context_info in train_loader :
            inp_ = inp_.to(device).requires_grad_(True)
            loss = model(inp_,mask.to(device),residuals.to(device),context_info)
            optim.zero_grad()
            loss['mae'].backward()
            optim.step()
            iteration += 1
            train_error += float(loss['mae'].cpu())
        if (True):
            model.eval()
            loss_mre_num,count = 0,0
            with torch.no_grad():
                for inp_,mask,residuals,context_info in val_loader :
                    loss = model.validate(inp_.to(device),mask.to(device),
                                          residuals.to(device),context_info)
                    loss_mre_num += (loss['loss_values']).sum()
                    count += len(loss['loss_values'])
            if (float(loss_mre_num)/count < 0.99*best_loss):
                tolerance_epoch = 0
                best_loss = float(loss_mre_num)/count
            elif (float(loss_mre_num)/count < best_loss):
                best_state_dict = model.state_dict()
                tolerance_epoch += 1
            else :
                tolerance_epoch += 1
            print ('done validation, Patience : ',tolerance_epoch)
            print ('validation loss : ',float(loss_mre_num/count))
            print ('train loss : ',float(train_error/interval))
            model.train()
            train_error = 0
            if (tolerance_epoch >= patience):
                print ('Early Stopping')
                return best_state_dict
    return best_state_dict

def test(model,test_loader,val_feats,device):
    output_matrix = copy.deepcopy(val_feats)
    model.eval()
    with torch.no_grad():
        for inp_,mask,residuals,context_info in test_loader :
            loss = model.validate(inp_.to(device),mask.to(device),residuals.to(device),context_info,test=True)
            output_matrix[context_info[1][0]:context_info[1][0]+mask.shape[1],context_info[0][0,0]] = \
            np.where(mask.detach().cpu().numpy()[0],loss['values'].detach().cpu().numpy()[0],output_matrix[context_info[1][0]:context_info[1][0]+mask.shape[1],context_info[0][0,0]])
    model.train()
    return output_matrix


def transformer_recovery(input_feats):
    torch.backends.cudnn.benchmark = False
    torch.backends.cudnn.deterministic = True
    torch.manual_seed(0)
    torch.cuda.manual_seed(0)
    torch.cuda.manual_seed_all(0)
    device = torch.device('cpu')

    np.random.seed(0)
    random.seed(0)

    print ('start')
    use_embed=True
    use_context=True
    use_local = False

    global interval

    mean = np.nanmean(input_feats,axis=0)
    std = np.nanstd(input_feats,axis=0)
    input_feats = (input_feats-mean)/std
    num_missing = 10*min(max(int(input_feats.shape[0]/100),1),500)
    train_feats,val_feats,val_points,test_points,block_size,kernel_size = utils.make_validation(input_feats,num_missing=num_missing)
    
    if (block_size > 100):
        kernel_size = 20

    time_context = min(int(input_feats.shape[0]/2),30*kernel_size)

    use_embed= (not utils.is_blackout(input_feats))
    use_context=(block_size <= kernel_size)
    use_local = (block_size < kernel_size)

    
    print ('Block size is %d, kernel size is %d'%(block_size,kernel_size))


    print ('Use Kernel Regression : ',use_embed)
    print ('Use Context in Keys : ',use_context)
    print ('Use Local Attention : ',use_local)

    batch_size = min(input_feats.shape[1]*int(input_feats.shape[0]/time_context),16)
    batch_size = input_feats.shape[1]
    interval = 1000
       
    train_set = myDataset(train_feats,use_local,time_context = time_context)
    val_set = myValDataset(val_feats,val_points,False,use_local,time_context = time_context)
    test_set = myValDataset(val_feats,test_points,True,use_local,time_context = time_context)
    train_loader = torch.utils.data.DataLoader(train_set,batch_size = batch_size,drop_last = False,shuffle=True,collate_fn = my_collate)
    val_loader = torch.utils.data.DataLoader(val_set,batch_size = batch_size,drop_last = False,shuffle=True,collate_fn = my_collate)
    test_loader = torch.utils.data.DataLoader(test_set,batch_size = 1,drop_last = False,shuffle=True,collate_fn = my_collate)

    model = OurModel(sizes=[train_feats.shape[1]],kernel_size=kernel_size,block_size = block_size,nhead=2,time_len=train_feats.shape[0],use_embed=use_embed,use_context=use_context,use_local=use_local).to(device)
    model.std = torch.from_numpy(std).to(device)

    best_state_dict = train(model,train_loader,val_loader,device)
    model.load_state_dict(best_state_dict)

    matrix = test(model,test_loader,val_feats,device)
    matrix = (matrix*std)+mean
    return matrix
