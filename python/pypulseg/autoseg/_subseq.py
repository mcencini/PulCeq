"""Identify subsequences (i.e., imaging, navigator, calibration, noise) in an unrolled loop."""

__all__ = ["segment_sequence"]

import numpy as np
from ._pattern import find_patterns


def segment_sequence(arr, min_length=1):
    subseq_fwd = find_patterns(arr, reverse=False)
    subseq_rev = find_patterns(arr, reverse=True)
    return _merge_segmentations(subseq_fwd, subseq_rev)


def _merge_segmentations(seg1, seg2):
    """Ensure consistent segmentation by merging two segmentations."""
    # If the segmentations are identical, return immediately
    if _is_equal(seg1, seg2):
        return [np.tile(s, r).tolist() for s, r in seg1]

    merged = []
    i, j = 0, 0

    while i < len(seg1) and j < len(seg2):
        s1, r1 = seg1[i]
        s2, r2 = seg2[j]

        # Expand the segments before comparison
        expanded_s1 = np.tile(s1, r1)
        expanded_s2 = np.tile(s2, r2)

        if np.array_equal(expanded_s1, expanded_s2):
            # If expanded sequences match, keep them
            merged.append(expanded_s1.tolist())
            i += 1
            j += 1
        else:
            # If they differ, extend the shorter one until they match
            combined_s1 = expanded_s1.copy()
            combined_s2 = expanded_s2.copy()

            while not np.array_equal(combined_s1, combined_s2):
                if len(combined_s1) < len(combined_s2):
                    i += 1
                    if i < len(seg1):
                        s1, r1 = seg1[i]
                        expanded_s1 = np.tile(s1, r1)
                        combined_s1 = np.concatenate([combined_s1, expanded_s1])
                else:
                    j += 1
                    if j < len(seg2):
                        s2, r2 = seg2[j]
                        expanded_s2 = np.tile(s2, r2)
                        combined_s2 = np.concatenate([combined_s2, expanded_s2])

            # Once they match, save the merged segment
            merged.append(combined_s1.tolist())
            i += 1
            j += 1

    return merged


def _is_equal(seg1, seg2):
    if len(seg1) != len(seg2):
        return False
    for n in range(len(seg1)):
        if seg1[n][1] != seg2[n][1]:
            return False
        if np.array_equal(seg1[n][0], seg2[n][0]) is False:
            return False
