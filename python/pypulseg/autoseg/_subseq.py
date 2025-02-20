"""Identify subsequences (i.e., imaging, navigator, calibration, noise) in an unrolled loop."""

__all__ = ["segment_sequence"]

import numpy as np
from ._pattern import find_patterns


def segment_sequence(arr, min_length=1):
    """
    Identify repeating subsequences in a sequence.

    This function detects patterns in `arr` and merges forward and reverse
    segmentations for consistency. If only one segment is found, it applies
    additional heuristics to identify periodicity.

    Parameters
    ----------
    arr : np.ndarray
        Input sequence to analyze.
    min_length : int, optional
        Minimum candidate length to consider (default is 1).

    Returns
    -------
    sections_def : list of np.ndarray
        List of unique segment definitions.
    sections_lut : np.ndarray
        Lookup table mapping elements of `arr` to segment IDs.

    """
    arr = np.asarray(arr).flatten()  # Ensure 1D NumPy array

    # Identify segments using forward and reverse pattern detection
    subseq_fwd = find_patterns(arr, reverse=False, min_length=min_length)
    subseq_rev = find_patterns(arr, reverse=True, min_length=min_length)

    # Merge segmentations for consistency
    merged_segments = _merge_segmentations(subseq_fwd, subseq_rev)
    n_segments = len(merged_segments)

    # Handle single-segment case early
    if n_segments == 1:
        sections_def, sections_lut = _find_sections_def(merged_segments[0])
        return np.asarray(sections_def).squeeze().tolist(), sections_lut

    # Process multiple segments
    sections_def = []
    sections_lut = []
    offset = 0

    for segment in merged_segments:
        current_def, current_lut = _find_sections_def(segment)

        # Append unique segment definitions
        sections_def.extend(current_def)

        # Adjust and append lookup table
        sections_lut.append(current_lut + offset)
        offset = (current_lut + offset).max() + 1

    # Cleanup
    sections_def = [section.squeeze().tolist() for section in sections_def]

    return sections_def, np.concatenate(sections_lut)


# %% local subroutines
def _merge_segmentations(seg1, seg2):
    """Ensure consistent segmentation by merging two segmentations."""
    # If the segmentations are identical, return immediately
    if _is_equal(seg1, seg2):
        merged = [np.tile(s, r).tolist() for s, r in seg1]
    else:
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


def _find_sections_def(arr):
    arr = np.asarray(arr).ravel()  # Ensure 1D array
    first_id = arr[0]

    # Find all occurrences of first_id
    indices = np.where(arr == first_id)[0]

    if len(indices) < 2:
        return [arr], np.zeros_like(arr, dtype=int)

    # Compute section lengths (differences between consecutive occurrences of first_id)
    section_lengths = np.concatenate([np.diff(indices), [len(arr) - indices[-1]]])

    # Identify unique section lengths and map them
    _, idx, section_IDs = np.unique(
        section_lengths, return_index=True, return_inverse=True
    )
    sorted_order = np.argsort(idx)
    unique_lengths = section_lengths[idx[sorted_order]]
    section_IDs = sorted_order[section_IDs]

    # Initialize outputs
    sections_def = []
    sections_lut = np.zeros_like(arr, dtype=int)

    offset = 0
    for i, section_len in enumerate(unique_lengths):
        # Get all section start indices corresponding to this length
        section_start_idx = indices[np.where(section_IDs == i)[0]]

        # Generate indices for all occurrences of this section
        all_section_idx = section_start_idx[:, None] + np.arange(section_len)
        all_section_idx = all_section_idx.ravel()  # Flatten index array

        # Extract section data
        section_data = arr[all_section_idx].reshape(
            -1, section_len
        )  # Shape: (num_sections, section_len)

        # Identify unique section patterns
        _, _idx, unique_section_IDs = np.unique(
            section_data, axis=0, return_index=True, return_inverse=True
        )
        sorted_order = np.argsort(_idx)
        unique_sections = section_data[_idx[sorted_order]]
        unique_section_IDs = sorted_order[unique_section_IDs]

        # Store unique sections
        sections_def.append(unique_sections)

        # Assign section IDs in lookup table
        sections_lut[all_section_idx] = (
            np.repeat(unique_section_IDs, section_len) + offset
        )  # +1 to keep IDs nonzero

        # Update offset
        offset = (unique_section_IDs + offset).max() + 1

    return sections_def, sections_lut


def _is_equal(seg1, seg2):
    if len(seg1) != len(seg2):
        return False
    for n in range(len(seg1)):
        if seg1[n][1] != seg2[n][1]:
            return False
        if np.array_equal(seg1[n][0], seg2[n][0]) is False:
            return False
