import argparse
import numpy as np
import gurobipy as gp
import pathlib
import itertools
import os


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Model for SRR")
    parser.add_argument("instance")
    args = parser.parse_args()

    for arg, val in args.__dict__.items():
        print(f"{arg}: {val}")

    path = pathlib.Path(args.instance)
    assert path.exists(), "Path does not exist."

    with open(path, "r") as f:
        first = True
        for line in f:
            if first:
                nteams = int(line)
                nrounds = nteams - 1
                coefs = np.zeros((nteams, nteams, nrounds))
                first = False
            else:
                if not line.strip():
                    continue
                i, j, r, cost = line.split()
                i, j, r = map(int, (i, j, r))
                cost = float(cost)
                coefs[i, j, r] = cost
                # coefs[j, i, r] = cost  # both directions are specified in the file.

    # print(coefs)

    model = gp.Model(name=path.stem)

    nthreads = os.environ.get("SLURM_CPUS_PER_TASK", 0)
    nthreads = int(nthreads) if nthreads else 0
    print(f"# Setting thread count to {nthreads}")
    model.setParam(gp.GRB.Param.Threads, nthreads)

    model.setParam(gp.GRB.Param.TimeLimit, 48*3600)

    teams = list(range(nteams))
    matches = list(itertools.combinations(teams, 2))
    rounds = list(range(nrounds))

    x = model.addVars(
        ((i, j, r) for (i, j) in matches for r in rounds),
        vtype=gp.GRB.BINARY
    )

    model.addConstrs(
        gp.quicksum(x[i, j, r] for r in rounds) == 1
        for (i, j) in matches
    )

    model.addConstrs(
        gp.quicksum(x[i, j, r] for (i, j) in matches if i_ in [i, j]) == 1
        for i_ in teams for r in rounds
    )

    model.setObjective(
        gp.quicksum(coefs[i, j, r] * x[i, j, r] for (i, j, r) in x),
        sense=gp.GRB.MINIMIZE
    )

    model.optimize()
