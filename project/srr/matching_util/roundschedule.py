from typing import List, Tuple


class Matching(object):
    glob_counter = 0

    def __init__(self, round, nteams):
        self._matches : List[Tuple[int, int]] = []
        self._opponent : List[int] = [-1] * nteams
        self._id = Matching.glob_counter
        self._round = round
        self._var = None
        Matching.glob_counter += 1

    def add_match(self, i : int, j : int):
        self._matches.append((i, j))
        assert self._opponent[i] == -1
        assert self._opponent[j] == -1
        self._opponent[i] = j
        self._opponent[j] = i

    def get_matches(self) -> List[Tuple[int, int]]:
        return self._matches

    def contains(self, i, j) -> bool:
        return self._opponent[i] == j

    def add_var(self, var):
        assert self._var is None
        self._var = var

    def get_round(self):
        return self._round

    def get_index(self):
        return self._id

    def __str__(self):
        return f"<M{self._id}@{self._round}: {str(self._matches).replace(' ', '')}>"

    def __repr__(self):
        return str(self)

    def __hash__(self):
        return self._id

    def __eq__(self, other):
        return self._id == other._id
