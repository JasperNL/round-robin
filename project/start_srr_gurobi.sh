#!/bin/bash
#SBATCH --partition=mcs.default.q
#SBATCH --cpus-per-task=2

source $HOME/prepare_scip_dev.sh
#source $HOME/prepare_gurobi.sh
source $HOME/python-venv/bin/activate

python3 -u solve_srr_gurobi.py $1
