"""Identify periodic subsequences in a unrolled sequence loop."""

import numpy as np

def _principal_period_bytes(s):
    """
    Finds the smallest repeating period in a bytes representation.

    Parameters
    ----------
    s : bytes
        The bytes representation of a sequence.

    Returns
    -------
    bytes or None
        The smallest repeating period if found, otherwise None.
    """
    i = (s + s).find(s, 1, -1)
    return None if i == -1 else s[:i]


def _find_periodic_pattern(arr):
    """
    Detects a periodic pattern in the given sequence.

    Parameters
    ----------
    arr : numpy.ndarray
        The input sequence of numerical identifiers.

    Returns
    -------
    tuple
        A tuple (loop, start_index), where:
        - loop : numpy.ndarray or None
            The detected periodic loop, if any.
        - start_index : int or None
            The starting index of the periodic pattern in arr, or None if not found.
    """
    numel = len(arr)
    for start in range(numel):
        loop_bytes = _principal_period_bytes(arr[start:].tobytes())
        if loop_bytes is not None:
            loop = np.frombuffer(loop_bytes, dtype=arr.dtype)
            if loop.size < (numel - start):  # Ensure non-trivial repetition
                return loop, start
    return None, None


def count_repetitions(arr, loop, start):
    """
    Counts consecutive repetitions of a detected periodic pattern.

    Parameters
    ----------
    arr : numpy.ndarray
        The input sequence.
    loop : numpy.ndarray
        The detected periodic loop.
    start : int
        The starting index of the periodic pattern.

    Returns
    -------
    int
        The number of consecutive repetitions of the loop.
    """
    period_len = loop.size
    count = 0
    while start + (count + 1) * period_len <= len(arr):
        segment = arr[start + count * period_len : start + (count + 1) * period_len]
        if np.array_equal(segment, loop):
            count += 1
        else:
            break
    return count


def segment_sequence(arr):
    """
    Recursively segments the input sequence into periodic and non-periodic subsequences.
    Returns both the unique definitions of subsequences and an array assigning each block
    to its corresponding subsequence.

    Parameters
    ----------
    arr : numpy.ndarray
        The input sequence of numerical identifiers.

    Returns
    -------
    tuple
        (subsequence_definitions, subsequence_mapping)
        
        - subsequence_definitions : list of numpy.ndarray
            List of unique subsequences (ordered sets of block IDs).
        - subsequence_mapping : numpy.ndarray
            Array (same length as arr) mapping each block to its subsequence ID.
    """
    subsequence_definitions = []

    def _recursive_segment(arr, start_idx):
        loop, start = _find_periodic_pattern(arr)
        
        if loop is None:
            # Entire array is non-periodic → assign it a new subsequence ID.
            subsequence_definitions.append(arr)
            return

        # Assign a non-periodic prefix if present.
        if start > 0:
            prefix = arr[:start]
            subsequence_definitions.append(prefix)

        # Count repetitions of detected periodic loop.
        rep = count_repetitions(arr, loop, start)
        
        # Store the periodic subsequence and assign mapping.
        subsequence_definitions.append(loop)

        # Recursively process any remaining elements.
        remainder_after = arr[start + rep * loop.size:]
        if remainder_after.size > 0:
            _recursive_segment(remainder_after, start_idx + start + rep * loop.size)

    # Start recursive segmentation.
    _recursive_segment(arr, 0)
    
    return subsequence_definitions