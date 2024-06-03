import argparse
import torch
import yaml
import time
import numpy as np

from models.pristi import PriSTI_
from utils import train, evaluate
import get_dataloader


def main(args, data):
    n, dim = data.shape
    SEED = args.seed
    np.random.seed(SEED)
    torch.manual_seed(SEED)
    torch.cuda.manual_seed(SEED)

    path = "config/" + args.config
    with open(path, "r") as f:
        config = yaml.safe_load(f)

    config["model"]["is_unconditional"] = args.unconditional
    config["model"]["target_strategy"] = args.targetstrategy
    config["diffusion"]["adj_file"] = 'airq'
    config["seed"] = SEED

    # print(json.dumps(config, indent=4))

    data_loader = get_dataloader.data_loader(data,
        is_interpolate=config["model"]["use_guide"], num_workers=args.num_workers,
        target_strategy=args.targetstrategy, mask_sensor=config["model"]["mask_sensor"],
        shuffle=False
    )

    config["train"]["batch_size"] = 1

    model = PriSTI_(config, args.device, target_dim=dim, seq_len=n).to(args.device)
    print("Starting training\n")
    train(
        model,
        config["train"],
        data_loader,
        valid_loader=data_loader
    )
    
    print("Starting evaluation\n")
    matrix = evaluate(
        model,
        data_loader,
        nsample=1,
        # scaler=scaler,
        # mean_scaler=mean_scaler,
    )

    return matrix


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="PriSTI")
    parser.add_argument("--input", type=str)
    parser.add_argument("--output", type=str)
    parser.add_argument("--runtime", type=int, default=0)

    parser.add_argument("--config", type=str, default="base.yaml")
    parser.add_argument('--device', default='cpu', help='Device for Attack')
    parser.add_argument('--num_workers', type=int, default=1, help='Device for Attack')
    parser.add_argument("--modelfolder", type=str, default="")
    parser.add_argument(
        "--targetstrategy", type=str, default="hybrid", choices=["hybrid", "random", "historical"]
    )
    parser.add_argument(
        "--val_len", type=float, default=0.1, help="the ratio of data used for validation (value:[0-1])"
    )
    parser.add_argument("--nsample", type=int, default=100)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--unconditional", action="store_true")

    args = parser.parse_args()

    IN = args.input
    OUT = args.output
    RT = args.runtime
    
    start = time.time()

    data = np.loadtxt(IN)

    matrix = main(args, data)

    print("done!")
    end = time.time()
    runtime = (end-start)*1000*1000

    print(f"Total runtime = {runtime/1e6} s")

    if RT:
        np.savetxt(OUT, np.array([runtime]))
    else:
        np.savetxt(OUT, matrix, delimiter=',', fmt='%f')
