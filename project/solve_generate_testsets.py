import pathlib
import itertools
import random
import srr
import numpy as np

n_nteams = [6, 12, 18, 24]
ratios =  [0.5, 0.6, 0.7, 0.8, 0.9]
seeds = range(50)

instancespath = pathlib.Path("instances/")
instancespath.mkdir(exist_ok=True)

for nteams, ratio, seed in itertools.product(n_nteams, ratios, seeds):
    path = instancespath / f"bin{nteams:03d}_{int(ratio*100):03d}_{seed:03d}.srr"
    costs = srr.get_random_costs(nteams, ratio, seed)

    with open(path, "w") as f:
        f.write(f"{nteams:d}\n")
        for (i, j, r), value in np.ndenumerate(costs):
            if np.abs(value) > 1e-12:
                f.write(f"{i:3d} {j:3d} {r:3d} {value:f}\n")

    print(f"Written {path}")
