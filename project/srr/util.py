import numpy as np


def sort_match(i, j):
    """
    In the world of matches, the matches are not sorted, but we work with sorted tuples.
    The order is that the first entry is smaller than the second.
    This utility function fixes (j, i) to (i, j) if i > j.
    """
    if i < j:
        return (i, j)
    else:
        assert j < i, "Team does not play itself."
        return (j, i)


def get_random_costs(nteams, ratio, seed) -> np.array:
    # Determine the cost matrix
    nrounds = nteams - 1
    nmatches = ((nteams * (nteams - 1)) // 2)
    nelements_costs = nmatches * nrounds
    nselected = int(nelements_costs * ratio)  # How many of the matches must we select?
    costs = np.zeros((nteams, nteams, nrounds), dtype=int)
    np.random.seed(seed)
    selected = 0
    choices_matches = np.random.choice(np.arange(nelements_costs), size=nselected, replace=False)
    lower_i, lower_j = np.where(np.arange(nteams)[:, np.newaxis] < np.arange(nteams)[np.newaxis, :])
    # Set the lower diagonal
    costs[lower_i[choices_matches % nmatches], lower_j[choices_matches % nmatches], choices_matches // nmatches] = 1
    # Symmetric matrix.
    costs[lower_j[choices_matches % nmatches], lower_i[choices_matches % nmatches], choices_matches // nmatches] = 1
    assert costs.sum() == nselected * 2
    return costs
