def experiment(tup):
    import srr
    import time

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

        model = models[variant](nteams, costs, integral=(variant in integral_models), timelimit=48*3600)

        model.optimize()

        stage = model.model.getStage()
        status = model.model.getStatus()
        objval = model.get_objval()

        model.free()
        tottime = time.time() - start
    except Exception as e:
        print(f"{tup}:\n{e}\n")
        stage, status, objval, tottime = None, None, -1.0, -1.0
    finally:
        print(f"@04 INFORMATION {(*tup, stage, status, objval, tottime)}\n")

    return (objval, status)

if __name__ == '__main__':
    import itertools
    import os
    import time
    import random

    # Make the list of all instances that we're going to run.
    variants = ["traditional", "matching", "traditional_integral"]
    n_nteams = [6, 12, 18, 24]
    ratios =  [0.5, 0.6, 0.7, 0.8, 0.9]
    seeds = range(50)
    arglist = [
        (variant, nteams, ratio, seed)
        for (variant, nteams, ratio, seed) in itertools.product(variants, n_nteams, ratios, seeds)
        # if not (variant == "traditional_integral" and nteams >= 24)
    ]

    # We use a slurm jobarray (see start_jobarray.sh). Each job processes one array element.
    jobid = os.environ.get("SLURM_ARRAY_JOB_ID")
    assert jobid is not None, f"No job ID present! There are {len(arglist)} possible arguments."
    idx = os.environ.get("SLURM_ARRAY_TASK_ID")
    assert idx is not None, "It is no job array!"

    idx = int(idx)
    args = arglist[idx]
    print(f"Solving for index {idx} out in {len(args)}")
    print(f"Arguments: {args}")

    print("Starting:")
    (obj, status) = experiment(args)
    print(f"Finished: {(args, (obj, status))}")

    filename = f"results/results{jobid}_{idx}.out"
    with open(filename, "w") as f:
        variant, nteams, ratio, seed = args
        f.write(f"{nteams:3d} {ratio:4.3f} {seed:3d} {variant:12s} {obj:e} {status:s}\n")