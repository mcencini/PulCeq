"""
Identify periodic patterns in a unrolled sequence loop with Numba acceleration.

This module recursively segments an input sequence into periodic and non‐periodic subsequences.
The inner candidate search and repetition‐counting loops are accelerated using Numba.
Each subsequence is represented as an ordered set of integer identifiers.
"""

__all__ = ["find_patterns"]

import numpy as np
from numba import njit


def find_patterns(arr, reverse=True, min_length=1):
    """
    Recursively segment the input sequence into periodic and non-periodic subsequences.

    Also count the number of consecutive repetitions for each detected periodic pattern.

    Parameters
    ----------
    arr : numpy.ndarray or list
        The input sequence of numerical identifiers.
    reverse : bool
        Search patterns starting from the end.
    min_length : int, optional
        Minimum candidate length to consider (default is 1).

    Returns
    -------
    list of tuples
        Each tuple contains:
        - A numpy.ndarray representing a subsequence
        - An integer representing the number of times the pattern repeats
    """
    if not isinstance(arr, (list, np.ndarray)) or len(arr) == 0:
        return []

    arr = np.asarray(arr)
    if reverse:
        arr = arr[::-1]  # Reverse for processing

    subsequence_definitions = []

    def _recursive_segment(subarr):
        if len(subarr) < min_length:
            subsequence_definitions.append((subarr, 1))
            return

        start, L = numba_find_periodic_pattern(subarr, min_length)
        if start == -1:
            subsequence_definitions.append((subarr, 1))
            return

        if start > 0:
            subsequence_definitions.append((subarr[:start], 1))  # Non-periodic prefix

        rep = numba_count_repetitions(subarr, start, L)
        subsequence_definitions.append(
            (subarr[start : start + L], rep)
        )  # Periodic pattern

        remainder = subarr[start + rep * L :]
        if remainder.size > 0:
            _recursive_segment(remainder)

    _recursive_segment(arr)

    if reverse:
        return [
            (sub[::-1].tolist(), rep) for sub, rep in reversed(subsequence_definitions)
        ]  # Restore order
    return [
        (sub.tolist(), rep) for sub, rep in subsequence_definitions
    ]  # Restore order


# %% local subroutines
@njit(cache=True)
def array_equal(a, b):
    """
    Compare two 1D arrays element-wise.

    Parameters
    ----------
    a : numpy.ndarray
        First array.
    b : numpy.ndarray
        Second array.

    Returns
    -------
    bool
        True if arrays are equal, False otherwise.
    """
    if a.shape[0] != b.shape[0]:
        return False
    for i in range(a.shape[0]):
        if a[i] != b[i]:
            return False
    return True


@njit(cache=True)
def numba_find_periodic_pattern(arr, min_length=1):
    """
    Search for a candidate periodic pattern in arr.

    For each possible starting index, this function considers candidate lengths
    (with a minimum defined by min_length). It selects the candidate with the maximum
    length (if any) that repeats immediately. If one is found, it returns its start index and length.

    Parameters
    ----------
    arr : numpy.ndarray
        Input 1D array.
    min_length : int, optional
        Minimum candidate length to consider (default is 1).

    Returns
    -------
    tuple of (int, int)
        (start, L) where 'start' is the index at which a periodic candidate is found,
        and L is its maximum length. Returns (-1, -1) if no candidate is found.
    """
    n = arr.shape[0]
    for start in range(n):
        max_possible_period = (n - start) // 2

        # Consider candidate lengths with a minimum of min_length
        for L in range(min_length, max_possible_period + 1):
            candidate = arr[start : start + L]
            candidate2 = arr[start + L : start + 2 * L]
            if array_equal(candidate, candidate2):
                return start, L

    return -1, -1


@njit(cache=True)
def numba_count_repetitions(arr, start, L):
    """
    Count consecutive repetitions of a candidate period.

    Parameters
    ----------
    arr : numpy.ndarray
        Input 1D array.
    start : int
        Starting index where the periodic candidate is detected.
    L : int
        Length of the candidate period.

    Returns
    -------
    int
        Number of consecutive repetitions of the candidate period.
    """
    count = 0
    n = arr.shape[0]
    while start + (count + 1) * L <= n:
        segment = arr[start + count * L : start + (count + 1) * L]
        base = arr[start : start + L]
        if array_equal(segment, base):
            count += 1
        else:
            break
    return count
