"""Automatic parent block finder."""

__all__ = ["find_parent_blocks"]

import copy
import math

from types import SimpleNamespace

import numpy as np
import pypulseq as pp


def find_parent_blocks(seq):
    blocks = np.stack(list(seq.block_events.values()), axis=1)
    
    # Get version
    version_combined = 1000000 * seq.version_major + 1000 * seq.version_minor + int(seq.version_revision)

    # Loop over [RF], extract (amp, mag_id, phase_id, time_id) -> row 0, 1, 2, 3 (0-index based)
    try:
        rf = np.stack(list(seq.rf_library.data.values()), axis=-1)
        rf_events = rf[:4].T
        rf_events = np.pad(rf_events, ((1, 0), (0, 0)))
    except Exception:
        rf_events = None

    # Split [GRADIENTS] and [TRAP]
    try:
        arbitrary_and_traps = np.array(list(seq.grad_library.data.values()), dtype=object)
        istrap = np.asarray([len(g) == 5 for g in arbitrary_and_traps])
    except Exception:
        istrap = np.asarray([], dtype=bool)
    isarbitrary = np.logical_not(istrap)

    # Actual split
    if isarbitrary.any():
        arbitrary = np.stack(list(arbitrary_and_traps[isarbitrary]), axis=-1)

        # Loop over [GRADIENTS], extract (amp, shape_id, time_id) -> row 0-2 (0-index based)
        if version_combined < 1005000:
            arbitrary_events = arbitrary[:3]
        else:
            # they break format in 1.5.0+, 
            # from (amp, shape_id, time_id, ...) to (amp, first, last, shape_id, time_id,...)
            arbitrary_events = np.asarray([arbitrary[0], arbitrary[3], arbitrary[4]])
        arbitrary_events = arbitrary_events.T
        
        # Pad so it is compatible with trapezoids
        arbitrary_events = np.pad(arbitrary_events, ((0, 0), (0, 2)))
    else:
        arbitrary_events = None

    if istrap.any():
        traps = np.stack(list(arbitrary_and_traps[istrap]), axis=-1)

        # Loop over [TRAP], extract (amp, rise, flat, fall, delay) -> all rows
        trap_events = traps.T
    else:
        trap_events = None

    # Put back together arbitrary gradients and traps
    isarbitrary_event = np.zeros(len(arbitrary_and_traps), dtype=bool)
    if arbitrary_events is not None and trap_events is not None:
        isarbitrary_event[isarbitrary] = True
        arbitrary_and_traps_events = np.zeros((len(arbitrary_and_traps), 5), dtype=float)
        arbitrary_and_traps_events[isarbitrary] = arbitrary_events.astype(float)
        arbitrary_and_traps_events[istrap] = trap_events.astype(float)
    elif arbitrary_events is not None:
        isarbitrary_event += 1
        arbitrary_and_traps_events = arbitrary_events.astype(float)
    elif trap_events is not None:
        arbitrary_and_traps_events = trap_events.astype(float)
    else:
        arbitrary_and_traps_events = None
        
    # Pad to account for 1-based index
    if arbitrary_and_traps_events is not None:
        isarbitrary_event = np.pad(isarbitrary_event, (1, 0))
        arbitrary_and_traps_events = np.pad(arbitrary_and_traps_events, ((1, 0), (0, 0)))

    # Loop over [ADC], extract (num, dwell, delay) -> -> row 1-3 (0-index based)
    try:
        adc = np.stack(list(seq.adc_library.data.values()), axis=-1)
        adc_events = adc[:3].T
        adc_events = np.pad(adc_events, ((1, 0), (0, 0)))
    except Exception:
        adc_events = None

    # Get rf, gx, gy, gz and adc indexes
    rf_idx = blocks[1]
    gx_idx = blocks[2]
    gy_idx = blocks[3]
    gz_idx = blocks[4]
    adc_idx = blocks[5]
    
    # Default values
    n_blocks = blocks.shape[1]
    rf_uid = np.zeros((n_blocks, 3), dtype=float)
    gx_uid = np.zeros((n_blocks, 4), dtype=float)
    gy_uid = np.zeros((n_blocks, 4), dtype=float)
    gz_uid = np.zeros((n_blocks, 4), dtype=float)
    adc_uid = np.zeros((n_blocks, 3), dtype=float)

    # Build block identifiers matrix
    # ------------------------------
    # Duration:
    dur =  np.asarray(list(seq.block_durations.values()))
    
    # RF: (nblocks, 3); row = [mag_id, phase_id, time_id]
    if rf_events is not None:
        rf_uid = rf_events[rf_idx, 1:]
    
    if arbitrary_and_traps_events is not None:
        # Gx: (nblocks, 4); row = [shape_id, time_id, 0, 0] (grad) or [rise, flat, fall, delay] (trap)
        gx_uid = arbitrary_and_traps_events[gx_idx, 1:]
        
        # Gy: (nblocks, 4); row = [shape_id, time_id, 0, 0] (grad) or [rise, flat, fall, delay] (trap)
        gy_uid = arbitrary_and_traps_events[gy_idx, 1:]
        
        # Gz: (nblocks, 4); row = [shape_id, time_id, 0, 0,] (grad) or [rise, flat, fall, delay] (trap)
        gz_uid = arbitrary_and_traps_events[gz_idx, 1:]
        
        # Gradient classification
        gx_isarbitrary = isarbitrary_event[gx_idx]
        gy_isarbitrary = isarbitrary_event[gy_idx]
        gz_isarbitrary = isarbitrary_event[gz_idx]
        hasarbitrary = gx_isarbitrary + gy_isarbitrary + gz_isarbitrary
        
        # Uncompress all waveforms
        shapes = list(seq.shape_library.data.values())
        n_samples = [int(shape[0]) for shape in shapes]
        
        # Collect number of points for each arbitrary waveform
        tx_shape = gx_uid[:, 1].astype(int) * hasarbitrary
        ty_shape = gy_uid[:, 1].astype(int) * hasarbitrary
        tz_shape = gz_uid[:, 1].astype(int) * hasarbitrary
        
        
        
                
    # ADC: (nblocks, 3); row = [num, dwell, delay]
    if adc_events is not None:
        adc_uid = adc_events[adc_idx]  
    
    # Put together RF, Gx, Gy, Gz, ADC and duration, trigger flag
    block_uid = [dur[:, None], rf_uid, gx_uid, gy_uid, gz_uid, adc_uid]
    block_uid = np.concatenate(block_uid, axis=1)

    # Find pure delay blocks
    # exclude duration and adc columns (isdelay.m only checks for rf, gx, gy and gz waveforms)
    notdelay = block_uid[:, 1:-3].any(axis=1)
    event_uid = block_uid[notdelay]

    # Find unique non-delay events in parent blocks
    parent_blocks_ids = _unique(event_uid)
    parent_blocks_def = block_uid[parent_blocks_ids]

    # generate parent block list and module index for each row in loop
    parent_blocks_idx = _get_parent_blocks(
        seq, block_uid, parent_blocks_def, parent_blocks_ids
    )
        
    return parent_blocks_def, parent_blocks_idx  


# %% local utils
def _get_parent_blocks(seq, block_uid, parent_block_uid, parent_blocks_ids):
    # Number of unique blocks
    n_parent_blocks = parent_block_uid.shape[0]
    parent_blocks_idx = -1 * np.ones(block_uid.shape[0], dtype=int)

    # Find indexes
    for n in range(n_parent_blocks):
        # Compare each row in the matrix with the given row
        tmp = np.all(block_uid == parent_block_uid[n], axis=1)

        # Find the index of the first matching row
        parent_blocks_idx[tmp] = n

    # Convert to array
    parent_blocks_idx = np.asarray(parent_blocks_idx)
    parent_blocks_idx += 1

    # Get blocks
    parent_blocks = []

    # Get shapes library
    shape_lib = list(seq.shape_library.data.values())

    for n in range(n_parent_blocks):
        idx = parent_blocks_ids[n] + 1
        parent_block = copy.deepcopy(seq.get_block(idx))
        if parent_block.rf is not None:
            mag_idx, phase_idx = int(block_uid[idx][1]), int(block_uid[idx][2])
            mag_shape = _decompress(shape_lib[mag_idx])
            phase_shape = _decompress(shape_lib[phase_idx])

            parent_block.rf.signal = mag_shape * np.exp(1j * 2 * math.pi * phase_shape)

        if parent_block.gx is not None and parent_block.gx.type == "grad":
            shape_idx = int(block_uid[idx][3])
            grad_shape = _decompress(shape_lib[shape_idx])

            parent_block.gx.waveform = grad_shape

        if parent_block.gy is not None and parent_block.gy.type == "grad":
            shape_idx = int(block_uid[idx][7])
            grad_shape = _decompress(shape_lib[shape_idx])

            parent_block.gy.waveform = grad_shape

        if parent_block.gz is not None and parent_block.gz.type == "grad":
            shape_idx = int(block_uid[idx][11])
            grad_shape = _decompress(shape_lib[shape_idx])

            parent_block.gz.waveform = grad_shape

        parent_blocks.append(parent_block)
        
    return parent_blocks_idx


def _decompress(shape_data):
    compressed = SimpleNamespace()
    compressed.num_samples = shape_data[0]
    compressed.data = shape_data[1:]

    return pp.decompress_shape.decompress_shape(compressed)


def _unique(event_uid):
    sort_idx = np.lexsort(event_uid.T[::-1])
    sorted_event_uid = event_uid[sort_idx]
    
    # Identify unique rows
    row_diff = np.any(np.diff(sorted_event_uid, axis=0), axis=1)
    unique_mask = np.concatenate(([True], row_diff))
    
    # Get the original indices of first occurrences
    parent_blocks_ids = sort_idx[unique_mask]
    
    # Sort them to match the behavior of sorted(parent_blocks_ids)
    return np.sort(parent_blocks_ids)


# def _unique(event_uid):
#     parent_blocks_ids = np.unique(event_uid, return_index=True, axis=0)[1]
#     return sorted(parent_blocks_ids)
