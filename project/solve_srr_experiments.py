import sys

class NullWriter:
    def write(self, s, *args):
        pass
    def flush(self):
        pass

devnull = NullWriter()
stdout = sys.stdout

def experiment(tup):
    import itertools
    import srr
    import time
    import argparse

    parser = argparse.ArgumentParser(description="Run experiments")
    parser.add_argument("--output", default=False, type=bool)
    args = parser.parse_args()

    if not args.output:
        sys.stdout = devnull


    try:
        start = time.time()
        variant, nteams, ratio, seed = tup
        costs = srr.get_random_costs(nteams, ratio, seed)

        models = {
            "traditional" : srr.traditional.Model,
            "traditional_integral" : srr.traditional.Model,
            "matching": srr.matching.Model,
            "permutation": srr.matching.Model
        }
        integral_models = {"traditional_integral"}

        model = models[variant](nteams, costs, integral=(variant in integral_models))

        model.optimize()

        stage = model.model.getStage()
        status = model.model.getStatus()
        objval = model.get_objval()

        model.free()
        tottime = time.time() - start
    except Exception as e:
        stdout.write(f"{tup}:\n{e}\n")
        stage, status, objval, tottime = None, None, -1.0, -1.0
    finally:
        stdout.write(f"{(*tup, stage, status, objval, tottime)}\n")

    return (objval, status)

if __name__ == '__main__':
    from multiprocessing import Pool
    import itertools
    import os
    import time
    import random

    variants = ["traditional", "matching", "traditional_integral"]
    n_nteams = [6, 12, 18, 24]
    ratios =  [0.5, 0.6, 0.7, 0.8, 0.9]
    seeds = range(50)

    arglist = list(itertools.product(variants, n_nteams, ratios, seeds))
    random.shuffle(arglist)

    idx = os.environ.get("SLURM_ARRAY_TASK_ID")
    if idx is not None:
        idx = int(idx)
        minidx = int(os.environ.get("SLURM_ARRAY_TASK_MIN"))
        maxidx = int(os.environ.get("SLURM_ARRAY_TASK_MAX"))
        maxidx -= minidx
        idx -= minidx
        numper = len(arglist) // maxidx
        arglist = list(arglist[idx * numper : (idx + 1) * numper])

    print("TODO:")
    for a in arglist:
        print(*a)

    print("Starting:")
    # len(os.sched_getaffinity(0))  # Number of cores reserved for usage.
    with Pool(processes=len(os.sched_getaffinity(0)), maxtasksperchild=8) as pool:
        results = pool.map(experiment, arglist)
        print(results)

    nowstr = time.strftime("%Y%m%d-%H%M%s")
    filename = f"results/results{nowstr}.out" if idx is None else f"results/results{nowstr}_{idx}.out"
    with open(filename, "w") as f:
        for tup, (obj, status) in zip(arglist, results):
            variant, nteams, ratio, seed = tup
            f.write(f"{nteams:3d} {ratio:4.3f} {seed:3d} {variant:12s} {obj:e} {status:s}\n")