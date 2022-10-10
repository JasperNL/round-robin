#!/bin/bash
#SBATCH --partition=moskito
#SBATCH --account=dopt
#SBATCH --exclusive
#SBATCH --mem=0

INST=$1
INSTNAME=$(basename $INST .srr)

mkdir -p check/results/
echo "Solving for ${INSTNAME}"
echo "Writing to check/results/${INSTNAME}.out and check/results/${INSTNAME}.err"
bin/srrsolver -f $1 -s settings/runtest.set > check/results/${INSTNAME}.out 2> check/results/${INSTNAME}.err