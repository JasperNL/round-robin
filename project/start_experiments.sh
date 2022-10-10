#!/bin/bash
#SBATCH --partition=mcs.default.q
#SBATCH --cpus-per-task=48
#SBATCH --array=0-9

source $HOME/prepare_scip_dev.sh
source $HOME/python-venv/bin/activate

python3 -u solve_srr_experiments.py
