# Python code for SRR

This folder consists of the following:

* `instances/`: The generated problem instances (link to `../instances/`)
* `srr/`: A python package containing all single-round robin code. The code is effectively split up in three parts: One for the traditional formulation, one for the permutation formulation, and one for the traditional formulation. These are all implementations in PySCIPOpt.
* `solve_generate_testsets.py`: Generating the instances in `instances/`.
* `solve_srr.py`: Based on the input, it generates an instance and runs it with the SRR model from `srr/`. For the arguments, run `python solve_srr.py --help`.
* `solve_srr_gurobi.py`: Given a problem instance (a `.srr`-file), build the traditional formulation and solve with Gurobi. For the arguments, run `python solve_srr_gurobi.py --help`.
* `solve_srr_experiments_jobarray.py`, `solve_srr_experiments.py`, `start_experiments.sh`, `start_jobarray.sh`, `start_srr_gurobi.sh`: Runscripts for running the code on the TU/e compute cluster.

The python code depends on the following packages

* PySCIPOpt
* NetworkX
* numpy