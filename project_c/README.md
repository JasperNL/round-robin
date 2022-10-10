# Matching formulation with pricer

Compilation instructions are at [`INSTALL.md`](INSTALL.md).

After installing, the shell can be opened by running `bin/srrsolver`.
This is the standard SCIP shell, and supports reading `.srr`-files: The files specifying round-robin tournaments.
For the specification on `.srr`-files, see [`README.md` at `instances/`](../instances/README.md)

## Example
Instances can be run with `bin/srrsolver -f instances/bin012_080_000.srr`

```log
SCIP version 8.0.1 [precision: 8 byte] [memory: block] [mode: optimized] [LP solver: Soplex 6.0.1.3] [GitHash: c84ee4283e]
Copyright (C) 2002-2022 Konrad-Zuse-Zentrum fuer Informationstechnik Berlin (ZIB)

External libraries:
  Readline 8.1         GNU library for command line editing (gnu.org/s/readline)
  Soplex 6.0.1.3       Linear Programming Solver developed at Zuse Institute Berlin (soplex.zib.de) [GitHash: 713be0fe]
  CppAD 20180000.0     Algorithmic Differentiation of C++ algorithms developed by B. Bell (github.com/coin-or/CppAD)
  ZLIB 1.2.11          General purpose compression library by J. Gailly and M. Adler (zlib.net)
  bliss 0.77           Computing Graph Automorphism Groups by T. Junttila and P. Kaski (www.tcs.hut.fi/Software/bliss/)

user parameter file <scip.set> not found - using default parameters

read problem <instances/bin012_080_000.srr>
============

Original problem:
Objective is integral
original problem has 0 variables (0 bin, 0 int, 0 impl, 0 cont) and 77 constraints

solve problem
=============

presolving:
presolving (1 rounds: 1 fast, 1 medium, 1 exhaustive):
 0 deleted vars, 0 deleted constraints, 0 added constraints, 0 tightened bounds, 0 added holes, 0 changed sides, 0 changed coefficients
 0 implications, 0 cliques
presolved problem has 0 variables (0 bin, 0 int, 0 impl, 0 cont) and 77 constraints
     77 constraints of type <linear>
transformed objective value is always integral (scale: 1)
Presolving Time: 0.00

 time | node  | left  |LP iter|LP it/n|mem/heur|mdpt |vars |cons |rows |cuts |sepa|confs|strbr|  dualbound   | primalbound  |  gap   | compl.
  0.0s|     1 |     0 |     1 |     - |   706k |   0 |   1 |  77 |  77 |   0 |  0 |   0 |   0 |      --      |      --      |    Inf | unknown
  0.1s|     1 |     0 |  2339 |     - |  3811k |   0 | 911 |  77 |  77 |   0 |  0 |   0 |   0 |      --      |      --      |    Inf | unknown
  0.1s|     1 |     0 |  2493 |     - |  4294k |   0 | 978 |  77 |  77 |   0 |  0 |   0 |   0 | 1.790079e+01 |      --      |    Inf | unknown
  0.9s|     1 |     2 | 34481 |     - |  6412k |   0 |1304 |  77 |  77 |   0 |  0 |   0 |   0 | 1.790079e+01 |      --      |    Inf | unknown
r 2.5s|     7 |     4 | 98176 |15860.5 |simplero|   6 |1179 |  77 |  77 |   0 |  0 |   0 |   0 | 1.790079e+01 | 2.000000e+01 |  11.73%|   7.31%
r 4.4s|    36 |     0 |163753 |4592.6 |simplero|   8 |1319 |  77 |  77 |   0 |  0 |   0 |   0 | 1.887137e+01 | 1.900000e+01 |   0.68%|  98.02%

SCIP Status        : problem is solved [optimal solution found]
Solving Time (sec) : 4.44
Solving Nodes      : 38
Primal Bound       : +1.90000000000000e+01 (2 solutions)
Dual Bound         : +1.90000000000000e+01
Gap                : 0.00 %

primal solution (transformed space):
====================================


Statistics
==========

SCIP Status        : problem is solved [optimal solution found]
Total Time         :       4.44
  solving          :       4.44
  presolving       :       0.00 (included in solving)
  reading          :       0.00
  copying          :       0.00 (1 #copies) (minimal 0.00, maximal 0.00, average 0.00)
Original Problem   :
  Problem name     : instances/bin012_080_000.srr
  Variables        : 0 (0 binary, 0 integer, 0 implicit integer, 0 continuous)
  Constraints      : 77 initial, 77 maximal
  Objective        : minimize, 0 non-zeros (abs.min = 1e+20, abs.max = -1e+20)
Presolved Problem  :
  Problem name     : t_instances/bin012_080_000.srr
  Variables        : 1319 (1319 binary, 0 integer, 0 implicit integer, 0 continuous)
  Constraints      : 77 initial, 77 maximal
  Objective        : minimize, 1314 non-zeros (abs.min = 1, abs.max = 6)
  Nonzeros         : 0 constraint, 0 clique table
Presolvers         :   ExecTime  SetupTime  Calls  FixedVars   AggrVars   ChgTypes  ChgBounds   AddHoles    DelCons    AddCons   ChgSides   ChgCoefs
  boundshift       :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  convertinttobin  :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  domcol           :       0.00       0.00      1          0          0          0          0          0          0          0          0          0
  dualagg          :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  dualcomp         :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  dualinfer        :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  dualsparsify     :       0.00       0.00      1          0          0          0          0          0          0          0          0          0
  gateextraction   :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  implics          :       0.00       0.00      1          0          0          0          0          0          0          0          0          0
  inttobinary      :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  qpkktref         :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  redvub           :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  sparsify         :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  stuffing         :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  trivial          :       0.00       0.00      1          0          0          0          0          0          0          0          0          0
  tworowbnd        :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  dualfix          :       0.00       0.00      1          0          0          0          0          0          0          0          0          0
  genvbounds       :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  probing          :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  pseudoobj        :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  symmetry         :       0.00       0.00      1          0          0          0          0          0          0          0          0          0
  vbounds          :       0.00       0.00      0          0          0          0          0          0          0          0          0          0
  linear           :       0.00       0.00      2          0          0          0          0          0          0          0          0          0
  root node        :          -          -      -       3047          -          -       3047          -          -          -          -          -
Constraints        :     Number  MaxNumber  #Separate #Propagate    #EnfoLP    #EnfoRelax  #EnfoPS    #Check   #ResProp    Cutoffs    DomReds       Cuts    Applied      Conss   Children
  integral         :          0          0          0          0         27          0          0         10          0          0          0          0          0          0         54
  srrmor           :          0+         8          0       1501         27          0          0          7          0          0       2227          0          0          0          0
  linear           :         77         77          0       1501          0          0          0          6          0          0          0          0          0          0          0
Constraint Timings :  TotalTime  SetupTime   Separate  Propagate     EnfoLP     EnfoPS     EnfoRelax   Check    ResProp    SB-Prop
  integral         :       3.97       0.00       0.00       0.00       3.97       0.00       0.00       0.00       0.00       0.00
  srrmor           :       0.05       0.00       0.00       0.05       0.00       0.00       0.00       0.00       0.00       0.00
  linear           :       0.00       0.00       0.00       0.00       0.00       0.00       0.00       0.00       0.00       0.00
Propagators        : #Propagate   #ResProp    Cutoffs    DomReds
  dualfix          :          1          0          0          0
  genvbounds       :          0          0          0          0
  nlobbt           :          0          0          0          0
  obbt             :          0          0          0          0
  probing          :          0          0          0          0
  pseudoobj        :          0          0          0          0
  redcost          :          0          0          0          0
  rootredcost      :          0          0          0          0
  symmetry         :          0          0          0          0
  vbounds          :          0          0          0          0
Propagator Timings :  TotalTime  SetupTime   Presolve  Propagate    ResProp    SB-Prop
  dualfix          :       0.00       0.00       0.00       0.00       0.00       0.00
  genvbounds       :       0.00       0.00       0.00       0.00       0.00       0.00
  nlobbt           :       0.00       0.00       0.00       0.00       0.00       0.00
  obbt             :       0.00       0.00       0.00       0.00       0.00       0.00
  probing          :       0.00       0.00       0.00       0.00       0.00       0.00
  pseudoobj        :       0.00       0.00       0.00       0.00       0.00       0.00
  redcost          :       0.00       0.00       0.00       0.00       0.00       0.00
  rootredcost      :       0.00       0.00       0.00       0.00       0.00       0.00
  symmetry         :       0.00       0.00       0.00       0.00       0.00       0.00
  vbounds          :       0.00       0.00       0.00       0.00       0.00       0.00
Conflict Analysis  :       Time      Calls    Success    DomReds  Conflicts   Literals    Reconvs ReconvLits   Dualrays   Nonzeros   LP Iters   (pool size: [--,--])
  propagation      :       0.00          0          0          -          0        0.0          0        0.0          -          -          -
  infeasible LP    :       0.00          0          0          -          0        0.0          0        0.0          0        0.0          0
  bound exceed. LP :       0.00          0          0          -          0        0.0          0        0.0          0        0.0          0
  strong branching :       0.00          0          0          -          0        0.0          0        0.0          -          -          0
  pseudo solution  :       0.00          0          0          -          0        0.0          0        0.0          -          -          -
  applied globally :       0.00          -          -          0          0        0.0          -          -          0          -          -
  applied locally  :          -          -          -          0          0        0.0          -          -          0          -          -
Separators         :   ExecTime  SetupTime      Calls    Cutoffs    DomReds       Cuts    Applied      Conss
  cut pool         :       0.00                     0          -          -          0          -          -    (maximal pool size: 0)
Cutselectors       :   ExecTime  SetupTime
  hybrid           :       0.00       0.00
Pricers            :   ExecTime  SetupTime      Calls       Vars
  problem variables:       0.10          -       5103     264509
  matchingpricer   :       0.77       0.00       1975       4185
Branching Rules    :   ExecTime  SetupTime   BranchLP  BranchExt   BranchPS    Cutoffs    DomReds       Cuts      Conss   Children
  matchingbranch   :       3.97       0.00         27          0          0          0          0          0          0         54
Primal Heuristics  :   ExecTime  SetupTime      Calls      Found       Best
  LP solutions     :       0.00          -          -          0          0
  relax solutions  :       0.00          -          -          0          0
  pseudo solutions :       0.00          -          -          0          0
  strong branching :       0.00          -          -          0          0
  alns             :       0.00       0.00          0          0          0
  bound            :       0.00       0.00          0          0          0
  clique           :       0.00       0.00          0          0          0
  coefdiving       :       0.00       0.00          0          0          0
  completesol      :       0.00       0.00          0          0          0
  crossover        :       0.00       0.00          0          0          0
  dins             :       0.00       0.00          0          0          0
  dps              :       0.00       0.00          0          0          0
  dualval          :       0.00       0.00          0          0          0
  feaspump         :       0.01       0.00          1          0          0
  fixandinfer      :       0.00       0.00          0          0          0
  gins             :       0.00       0.00          0          0          0
  indicator        :       0.00       0.00          0          0          0
  intshifting      :       0.00       0.00          0          0          0
  localbranching   :       0.00       0.00          0          0          0
  locks            :       0.00       0.00          0          0          0
  lpface           :       0.00       0.00          0          0          0
  mpec             :       0.00       0.00          0          0          0
  multistart       :       0.00       0.00          0          0          0
  mutation         :       0.00       0.00          0          0          0
  nlpdiving        :       0.00       0.00          0          0          0
  octane           :       0.00       0.00          0          0          0
  ofins            :       0.00       0.00          0          0          0
  oneopt           :       0.00       0.00          2          0          0
  padm             :       0.00       0.00          0          0          0
  proximity        :       0.00       0.00          0          0          0
  randrounding     :       0.00       0.00          1          0          0
  rens             :       0.00       0.00          1          0          0
  reoptsols        :       0.00       0.00          0          0          0
  repair           :       0.00       0.00          0          0          0
  rins             :       0.00       0.00          0          0          0
  rounding         :       0.00       0.00         27          0          0
  shiftandpropagate:       0.00       0.00          0          0          0
  shifting         :       0.00       0.00          1          0          0
  simplerounding   :       0.04       0.00       3908          2          2
  subnlp           :       0.00       0.00          0          0          0
  trivial          :       0.00       0.00          2          0          0
  trivialnegation  :       0.00       0.00          0          0          0
  trustregion      :       0.00       0.00          0          0          0
  trysol           :       0.00       0.00          0          0          0
  twoopt           :       0.00       0.00          0          0          0
  undercover       :       0.00       0.00          0          0          0
  vbounds          :       0.00       0.00          0          0          0
  zeroobj          :       0.00       0.00          0          0          0
  zirounding       :       0.00       0.00         25          0          0
  other solutions  :          -          -          -          0          -
Diving (single)    :      Calls      Nodes   LP Iters Backtracks  Conflicts   MinDepth   MaxDepth   AvgDepth  RoundSols  NLeafSols  MinSolDpt  MaxSolDpt  AvgSolDpt
  coefdiving       :          0          -          -          -          -          -          -          -          -          -          -          -          -
Diving (adaptive)  :      Calls      Nodes   LP Iters Backtracks  Conflicts   MinDepth   MaxDepth   AvgDepth  RoundSols  NLeafSols  MinSolDpt  MaxSolDpt  AvgSolDpt
  coefdiving       :          0          -          -          -          -          -          -          -          -          -          -          -          -
Neighborhoods      :      Calls  SetupTime  SolveTime SolveNodes       Sols       Best       Exp3  EpsGreedy        UCB TgtFixRate  Opt  Inf Node Stal  Sol  Usr Othr Actv
  rens             :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  rins             :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  mutation         :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  localbranching   :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  crossover        :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  proximity        :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  zeroobjective    :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  dins             :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
  trustregion      :          0       0.00       0.00          0          0          0    0.00000   -1.00000    1.00000      0.900    0    0    0    0    0    0    0    0
LP                 :       Time      Calls Iterations  Iter/call   Iter/sec  Time-0-It Calls-0-It    ItLimit
  primal LP        :       0.16        371       6055      19.79   38085.83       0.01         65
  dual LP          :       0.14        251       8040      45.17   56221.81       0.00         73
  lex dual LP      :       0.00          0          0       0.00          -
  barrier LP       :       0.00          0          0       0.00          -       0.00          0
  resolve instable :       0.00          1        197     197.00          -
  diving/probing LP:       2.90       4091     149672      36.59   51618.47
  strong branching :       0.00          0          0       0.00          -          -          -          0
    (at root node) :          -          0          0       0.00          -
  conflict analysis:       0.00          0          0       0.00          -
B&B Tree           :
  number of runs   :          1
  nodes            :         38 (27 internal, 11 leaves)
  feasible leaves  :          0
  infeas. leaves   :         11
  objective leaves :          0
  nodes (total)    :         38 (27 internal, 11 leaves)
  nodes left       :          0
  max depth        :          8
  max depth (total):          8
  backtracks       :         13 (34.2%)
  early backtracks :          0 (0.0%)
  nodes exc. ref.  :          0 (0.0%)
  delayed cutoffs  :          0
  repropagations   :         19 (0 domain reductions, 0 cutoffs)
  avg switch length:       6.32
  switching time   :       0.02
Estim. Tree Size   :         55
Estimation Tree    : 55 nodes (55 visited, 27 internal, 28 leaves, 0 open), weight: 1.0000 completed 1.0000
Estimations        :      estim      value      trend resolution     smooth
  wbe              :         55          -          -          -          -
  tree-profile     :         -1          -          -          -          -
  gap              :         55    1.00000    0.00965          1         55
  tree-weight      :         55    1.00000    0.01633          1         55
  leaf-frequency   :         55    0.50000    0.00210          1         56
  ssg              :         55    0.00000   -0.01997          1         55
  open-nodes       :         55    0.00000   -0.39026          1         56
Root Node          :
  First LP value   :          -
  First LP Iters   :          0
  First LP Time    :       0.00
  Final Dual Bound : +1.79007936507937e+01
  Final Root Iters :       3013
  Root LP Estimate : +1.79007936507937e+01
Solution           :
  Solutions found  :          2 (2 improvements)
  First Solution   : +2.00000000000000e+01   (in run 1, after 7 nodes, 2.50 seconds, depth 8, found by <simplerounding>)
  Gap First Sol.   :      11.73 %
  Gap Last Sol.    :       0.68 %
  Primal Bound     : +1.90000000000000e+01   (in run 1, after 36 nodes, 4.43 seconds, depth 8, found by <simplerounding>)
  Dual Bound       : +1.90000000000000e+01
  Gap              :       0.00 %
Integrals          :      Total       Avg%
  primal-dual      :     266.51      60.09
  primal-ref       :          -          - (not evaluated)
  dual-ref         :          -          - (not evaluated)
```
