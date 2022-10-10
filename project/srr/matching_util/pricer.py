import signal
import time
import itertools
import pyscipopt as ps
import networkx as nx
from typing import List, Set, Tuple, Dict
from srr.matching_util.roundschedule import Matching
from srr.util import sort_match
import numpy as np


class RoundMatchingPricer(ps.Pricer):
    model : ps.Model

    _fixings : np.array
    _feasible : bool
    _fixings_node : object

    def __init__(self, nteams : int, y : List[List[Matching]], conss : Tuple[Dict], costs : np.array) -> None:
        self._iter = 0
        self._teams = range(nteams)
        self._teams_to_shuffle = list(self._teams)
        self._rounds = range(nteams - 1)
        self._nteams = nteams
        self._nrounds = nteams - 1
        self._y = y
        self._conss = conss
        self._costs = costs

        self._interrupt = False
        signal.signal(signal.SIGTERM, self.interruptSolve)  # Sigterm doesn't work well with multiprocessing pools

        super().__init__()

    def interruptSolve(self, signum, frame):
        print("Received SIGTERM")
        self._interrupt = True

    def pricerfree(self):
        '''calls destructor and frees memory of variable pricer '''
        print("pricerfree")

    def pricerinit(self):
        '''initializes variable pricer'''
        print("pricerinit")
        pass

    def pricerexit(self):
        '''calls exit method of variable pricer'''
        print("pricerexit")
        pass

    def pricerinitsol(self):
        '''informs variable pricer that the branch and bound process is being started '''
        print("pricerinitsol")

    def pricerexitsol(self):
        '''informs variable pricer that the branch and bound process data is being freed'''
        print("pricerexitsol")
        pass

    def pricerredcost(self):
        '''calls reduced cost pricing method of variable pricer'''
        # print("pricerredcost")
        return self._solve_pricing(farkas=False)

    def pricerfarkas(self):
        '''calls Farkas pricing method of variable pricer'''
        # print("pricerfarkas")
        return self._solve_pricing(farkas=True)

    def _solve_pricing(self, farkas=False):
        # If we need to stop: Stop as soon as possible.
        if self._interrupt:
            self.model.interruptSolve()

        # Increase iteration count.
        self._iter = self._iter + 1

        ctr : ps.scip.Constraint
        dualsolfunc = self.model.getDualfarkasLinear if farkas else self.model.getDualsolLinear

        round_constrs, matching_constrs = self._conss

        u = {i : 0.0 for i in self._rounds}
        for i, ctr in round_constrs.items():
            assert i in u
            if not ctr.isModifiable():
                continue
            pi = dualsolfunc(ctr)
            if abs(pi) < 1e-6:
                continue
            u[i] = pi

        v = {(i, j): 0.0 for i, j in itertools.combinations(self._teams, r=2)}
        for (i, j), ctr in matching_constrs.items():
            assert (i, j) in v
            if not ctr.isModifiable():
                continue
            pi = dualsolfunc(ctr)
            if abs(pi) < 1e-6:
                continue
            v[i, j] = pi

        n_columns_added = 0
        for round in self._rounds:
            # Get new schedule for team.
            solval, matching = self._solve_round_subproblem(round, u, v, is_farkas=farkas)

            if (farkas and solval >= 0.0) or solval > 1e-6:
                assert matching is not None
                # Add column.
                self._add_round_matching(matching)
                n_columns_added += 1

        if n_columns_added > 0:
            return {"result": ps.SCIP_RESULT.SUCCESS}

        return {"result": ps.SCIP_RESULT.SUCCESS}

    def _solve_round_subproblem(self, r, u, v, is_farkas=False):
        """
        A dynamic programming approach to solve the problem.
        """
        g : nx.Graph = nx.Graph()

        # Add all possible edges
        if is_farkas:
            # Then the u and v-variables are rays. The costs cancel out by 'scaling'.
            for (i, j), v_val in v.items():
                g.add_edge(i, j, weight=v_val)
        else:
            for (i, j), v_val in v.items():
                g.add_edge(i, j, weight=v_val - self._costs[i, j, r])

        matching_edges = nx.algorithms.max_weight_matching(g, maxcardinality=True, weight="weight")
        matching_edges = [sort_match(i, j) for (i, j) in matching_edges]

        solval = None
        if is_farkas:
            # Then the u and v-variables are rays.
            solval = u[r] + sum(v[i, j] for (i, j) in matching_edges)
        else:
            solval = u[r] + sum(v[i, j] - self._costs[i, j, r] for (i, j) in matching_edges)

        matching = None
        if (is_farkas and solval >= 0.0) or solval > 1e-6:
            # Extract team schedule.
            matching = Matching(r, self._nteams)
            for (i, j) in matching_edges:
                matching.add_match(i, j)

        return solval, matching

    def _add_round_matching(self, matching : Matching):
        print(f"@03 Adding variable for matching {matching._id}: {matching.get_matches()}")
        # Create variable
        round_scheds = self._y[matching.get_round()]
        idx = len(round_scheds)
        r = matching.get_round()
        obj = sum(self._costs[i, j, r] for (i, j) in matching.get_matches())
        # We're interested in relaxations, so "C".
        if self._nteams <= 8:
            name = f"y[{r},{str(matching.get_matches()).replace(' ', '')}]"
        else:
            name = f"y[{r},{idx}]"
        var = self.model.addVar(name=name, vtype="C", pricedVar=True, obj=obj)
        matching.add_var(var)
        round_scheds.append(matching)
        self._y[r].append(matching)

        # Set coefficients
        round_constrs, matching_constrs = self._conss

        # u (round_constrs) constraint
        ctr = self.model.getTransformedCons(round_constrs[matching.get_round()])
        self.model.addConsCoeff(ctr, var, 1.0)

        # v (matching_constrs) constraints
        for (i, j) in matching.get_matches():
            ctr = self.model.getTransformedCons(matching_constrs[i, j])
            self.model.addConsCoeff(ctr, var, 1.0)
