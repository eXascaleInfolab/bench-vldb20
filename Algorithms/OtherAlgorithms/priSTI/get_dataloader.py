import pickle
from torch.utils.data import DataLoader, Dataset
import pandas as pd
import numpy as np
import torch
import torchcde
from utils import get_randmask, get_hist_mask

if not torch.cuda.is_available():
    torch.device('cpu')


class MyDataset(Dataset):
    def __init__(self, data, is_interpolate=False, target_strategy='hybrid', mask_sensor=None):
        self.data = data
        self.eval_len = data.shape[0]
        self.target_dim = data.shape[1]
        self.is_interpolate = is_interpolate
        self.target_strategy = target_strategy
        self.mask_sensor = mask_sensor

        self.mask = (~np.isnan(self.data)).astype(int)
        self.observed_data = np.nan_to_num(self.data)

    def __getitem__(self, idx):
        ob_data = self.observed_data
        ob_mask = self.mask
        ob_mask_t = torch.tensor(ob_mask).float()

        s = {
            "observed_data": np.array(ob_data),
            "observed_mask": np.array(ob_mask),
            "timepoints": np.array([np.arange(len(self.data))])

        }
        if self.target_strategy != 'random':
            cond_mask = get_hist_mask(ob_mask_t, for_pattern_mask=ob_mask)
            s["cond_mask"] = cond_mask
        else:
            cond_mask = get_randmask(ob_mask_t)
            s["cond_mask"] = cond_mask
        if self.is_interpolate:
            tmp_data = torch.tensor(ob_data).to(torch.float64)
            itp_data = torch.where(cond_mask == 0, float('nan'), tmp_data).to(torch.float32)
            itp_data = torchcde.linear_interpolation_coeffs(
                itp_data.unsqueeze(-1)).squeeze(-1)
            s["coeffs"] = np.array([itp_data.numpy()])    
        return s

    def __len__(self):
        return 1


def data_loader(data, is_interpolate=False, num_workers=4, target_strategy='hybrid', mask_sensor=None, shuffle=False):
    n, dim = data.shape
    dataset = MyDataset(data, is_interpolate, target_strategy, mask_sensor)
    data_loader = DataLoader(dataset, batch_size=1, num_workers=num_workers, shuffle=shuffle)
    return data_loader

    

