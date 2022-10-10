import pyscipopt as ps
import itertools
import numpy as np
from .util import sort_match
from typing import List, Set, Dict


class Model:
    def __init__(self, nteams, costs, integral=False, **kwargs) -> None:
        assert costs.shape == (nteams, nteams, nteams-1), "Incorrect dimensions"

        self.model : ps.Model = None
        self.nteams : int = nteams
        self.costs : np.array = costs

        self._build(integral=integral, **kwargs)

    def _build(self, integral=False, timelimit=None):
        assert self.model is None, "Model already initialized!"

        self.model = ps.Model(f"Minimize carry-over effect for {self.nteams} teams", enablepricing=False)
        self.model.redirectOutput()
        self.model.setParam("display/freq", 1)

        teams = range(self.nteams)
        rounds = range(self.nteams - 1)
        matches = list(itertools.combinations(teams, r=2))

        # Variables
        x = {
            # If we're only interested in relaxations, use "C".
            ((i, j), r): self.model.addVar(name=f"x[{i},{j},{r}]", lb=0.0, ub=1.0,
                vtype=("B" if integral else "C"))
            for (i, j), r in itertools.product(matches, rounds)
        }

        # Objective
        self.model.setObjective(
            ps.quicksum(
                cost * x[(i, j), r]
                for (i, j, r), cost in np.ndenumerate(self.costs)
                if np.abs(cost) > 1e-6 and i < j
            ),
            sense="minimize"
        )

        # Constraints
        for (i, j) in matches:
            self.model.addCons(
                ps.quicksum(x[(i, j), r] for r in rounds) == 1
            )


        for i, r in itertools.product(teams, rounds):
            self.model.addCons(
                ps.quicksum(x[sort_match(i, j), r] for j in teams if i != j) == 1
            )

        # Time limit if it's integral.
        if integral and timelimit is not None:
            self.model.setParam('limits/time', timelimit)

    def optimize(self):
        self.model.optimize()
        self.model.printStatistics()

    def get_objval(self):
        return self.model.getObjVal()

    def free(self):
        self.model.freeProb()
