#!/bin/bash

for INST in $(ls -1 instances/*.srr | grep -E 'bin(006|012|018)_.*\.srr')
do
    echo sbatch check/run_cluster.sh $INST
    sbatch check/run_cluster.sh $INST
done
