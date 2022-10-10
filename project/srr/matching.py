import pyscipopt as ps
import itertools
import numpy as np
from typing import List, Set, Dict
from srr.util import sort_match
from srr.matching_util import Matching, RoundMatchingPricer


class Model:
    def __init__(self, nteams, costs, **kwargs) -> None:
        assert costs.shape == (nteams, nteams, nteams-1), "Incorrect dimensions"

        self.model : ps.Model = None
        self.pricer = ps.Pricer = None
        self._y : List[List[Matching]] = None
        self.nteams : int = nteams
        self.costs : np.array = costs

        self._build()

    def _build(self):
        assert self.model is None, "Model already initialized!"

        self.model = ps.Model(f"Minimize carry-over effect for {self.nteams} teams", enablepricing=True)
        self.model.redirectOutput()
        self.model.setParam("display/freq", 1)

        teams = range(self.nteams)
        rounds = range(self.nteams - 1)
        matches = list(itertools.combinations(teams, r=2))

        # Variables
        self._y = [[] for _ in rounds]

        # Objective
        self.model.setObjective(
            0.0,
            sense="minimize"
        )

        # Constraints
        round_constrs = {
            r: self.model.addCons(
                ps.quicksum([]) == 1,
                modifiable = True
            )
            for r in rounds
        }

        match_constrs = {
            m: self.model.addCons(
                ps.quicksum([]) == 1,
                modifiable = True
            )
            for m in matches
        }

        # Introduce pricer
        pricer = self.pricer = RoundMatchingPricer(self.nteams, self._y, (round_constrs, match_constrs), self.costs)
        self.model.includePricer(pricer, "RoundMatchingPricer", "A pricer for generating round schedules.")

    def optimize(self):
        self.model.optimize()
        self.model.printStatistics()

    def get_objval(self):
        return self.model.getObjVal()

    def free(self):
        self.pricer.pricerfree()
        self.model.freeProb()
