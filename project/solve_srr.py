import srr
import argparse
import numpy as np


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Model for SRR")
    parser.add_argument("nteams", default=8, type=int, nargs="?")
    parser.add_argument("--variant", default="traditional", type=str)
    parser.add_argument("--seed", default=0, type=int)
    parser.add_argument("--ratio", default=0.2, type=float)

    args = parser.parse_args()

    for arg, val in args.__dict__.items():
        print(f"{arg}: {val}")

    assert args.nteams % 2 == 0, "Odd number of teams"

    models = {
        "traditional" : srr.traditional.Model,
        "matching": srr.matching.Model,
        "permutation": srr.matching.Model
    }

    assert args.variant in models.keys(), f"Unknown variant {args.variant}."
    assert 0 <= args.ratio <= 1, f"Ratio out of bounds."
    nteams = args.nteams

    costs = srr.get_random_costs(args.nteams, args.ratio, args.seed)

    if nteams <= 6:
        # Integer small enough to see things.
        ones = np.where(costs)
        print(ones)
        print(list(zip(*ones)))

    model = models[args.variant](nteams, costs)

    model.optimize()
