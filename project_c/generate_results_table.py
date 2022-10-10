from cmath import inf
import re
import pathlib
import argparse
import itertools
import numpy as np
import pandas as pd
import matplotlib.figure as mplfig
import matplotlib.pyplot as plt
import sys



if __name__ == "__main__":
    path = pathlib.Path("results/")

    instancepattern = re.compile(r"read problem <instances\/([^>]*)>")
    instancenamepattern = re.compile(r"bin(\d{3})_(\d{3})_(\d{3})\.srr")
    statuspattern = re.compile(r"SCIP Status        : ([^\[]+)\s\[([^\]]+)\]")
    primalpattern = re.compile(r"Primal Bound       : ([^\s]+)")
    dualpattern = re.compile(r"Dual Bound         : ([^\s]+)")
    timepattern = re.compile(r"Solving Time \(sec\) : ([^\s]+)")

    datas = []

    for filepath in path.glob("*.out"):
        print(filepath)

        with open(filepath, "r") as f:
            file_contents = f.read()

        failed = False

        m = instancepattern.search(file_contents)
        if m is None:
            failed = True
            instance = "???"
        else:
            instance = m.group(1)
        print(instance)

        m = instancenamepattern.match(instance)
        if m is None:
            instancedata = -1, 0.0, -1
        else:
            instancedata = int(m.group(1)), int(m.group(2))/100, int(m.group(3))
        print(instancedata)

        m = statuspattern.search(file_contents)
        if m is None:
            failed = True
            status = "ERROR", "ERROR"
        else:
            status = m.group(1), m.group(2)
        print(status)

        m = primalpattern.search(file_contents)
        if m is None:
            failed = True
            primal = float("-inf")
        else:
            primal = float(m.group(1))

        m = dualpattern.search(file_contents)
        if m is None:
            failed = True
            dual = float("inf")
        else:
            dual = float(m.group(1))
        print(primal, dual)

        m = timepattern.search(file_contents)
        if m is None:
            failed = True
            solvingtime = float("inf")
        else:
            solvingtime = float(m.group(1))
        print(primal, dual)

        datas.append((instance, *instancedata, status[0], status[1], primal, dual, solvingtime))

        print()

    # datas.sort(key=lambda tup: tup[0])
    df = pd.DataFrame(datas, columns=["instance", "nteams", "ratio", "seed", "status", "statusreason", "primal", "dual", "solvingtime"])

    df["normaloptimal"] = (df["statusreason"] == "optimal solution found")
    df["integraloptimal"] = ((-1e-6 < df["primal"] - df["dual"]) & (df["primal"] - df["dual"] <= 1.0 - 1e-6))
    df["crashed"] = df["status"] == "ERROR"
    df["memorylimit"] = (df["statusreason"] == "memory limit reached") & (~df["integraloptimal"])
    df["timelimit"] = (df["statusreason"] == "time limit reached") & (~df["integraloptimal"])

    print(df)

    # with pd.option_context("display.max_columns", None, "display.expand_frame_repr", False, "display.max_rows", None):
    #     print(
    #         df[df["integraloptimal"]]
    #     )

    print(
        df["integraloptimal"].sum(),
        (df["statusreason"] == "optimal solution found").sum()
    )

    print(
        df.groupby(["nteams", "ratio"])["normaloptimal", "integraloptimal", "crashed", "memorylimit", "timelimit", "solvingtime"].sum()
    )
    print()

    # Make textual table
    print("  n rat  #  O  T     min shgeomean   max")
    for (nteams, ratio), gdf in df.groupby(["nteams", "ratio"]):
        n = gdf["normaloptimal"].count()
        n_optimal = gdf["normaloptimal"].sum()
        n_integraloptimal = gdf["integraloptimal"].sum()
        assert n_optimal == n_integraloptimal, "We didn't specify that the objective is integral in code!"
        n_timelimit = gdf["timelimit"].sum()

        min_time = gdf["solvingtime"].min()
        geom_time = np.exp(np.mean(np.log(gdf["solvingtime"] + 10))) - 10
        max_time = gdf["solvingtime"].max()

        print(f"{nteams:3d} {ratio:3.1f} {n:2d} {n_optimal:2d} {n_timelimit:2d} {min_time:7.2f} {geom_time:7.2f} {max_time:7.2f}")
    print()

    # Same table, but now in LaTeX
    print(r"""\begin{tabular}{@{}rrrrrrrr@{}}
\toprule
&&&\multicolumn{2}{c}{Solved}&
\multicolumn{3}{c}{Solving time}\\
\cmidrule(l{4pt}r{4pt}){4-5}
\cmidrule(l{4pt}r{4pt}){6-8}
$n$ & $\rho$ & \# & O & T & min & mean & max \\
\midrule"""
    )

    for (nteams, ratio), gdf in df.groupby(["nteams", "ratio"]):
        n = gdf["normaloptimal"].count()
        n_optimal = gdf["normaloptimal"].sum()
        n_integraloptimal = gdf["integraloptimal"].sum()
        assert n_optimal == n_integraloptimal, "We didn't specify that the objective is integral in code!"
        n_timelimit = gdf["timelimit"].sum()

        min_time = gdf["solvingtime"].min()
        geom_time = np.exp(np.mean(np.log(gdf["solvingtime"] + 10))) - 10
        max_time = gdf["solvingtime"].max()

        print(f"{nteams:3d}&{ratio:3.1f}&{n:2d}&{n_optimal:2d}&{n_timelimit:2d}&{min_time:8.2f}&{geom_time:8.2f}&{max_time:8.2f}\\\\")

    print(
        r'\bottomrule'
        "\n"
        r'\end{tabular}'
    )

    # # Make scatterplot
    # fig = plt.figure()
    # # plt.yscale("log")

    # xpos = {6: 1, 12: 2, 18: 3, 24: 4}
    # ratiodiff = {.5: -.2, .6: -.1, .7: .0, .8: .1, .9: .2}
    # colors = {}
    # for (nteams, ratio), gdf in df.groupby(["nteams", "ratio"]):
    #     x = xpos[nteams] + ratiodiff[ratio]
    #     c = "r"

    #     n = gdf["normaloptimal"].count()
    #     n_optimal = gdf["normaloptimal"].sum()
    #     n_integraloptimal = gdf["integraloptimal"].sum()
    #     assert n_optimal == n_integraloptimal, "We didn't specify that the objective is integral in code!"
    #     n_timelimit = gdf["timelimit"].sum()

    #     min_time = gdf["solvingtime"].min()
    #     geom_time = np.exp(np.mean(np.log(gdf["solvingtime"] + 10))) - 10
    #     max_time = gdf["solvingtime"].max()

    #     plt.scatter([x] * n, gdf["solvingtime"].values + 1e-6, s=1, c=c)
    # plt.show()