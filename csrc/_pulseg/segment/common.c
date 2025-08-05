/**
 * @file common.c
 * @brief Utility routines for segmentation (autoseg and segparse).
 */

#include "common.h"

/**
 * @brief Harmonize gradient events in a segment to a common variant.
 *
 * This function checks all gradient events for a given axis in a segment,
 * determines the correct variant (TRAP, EXTENDED TRAP, ARBITRARY),
 * and returns the harmonized type and relevant parameters.
 *
 * Empty gradients are converted to zero-amplitude of the chosen type.
 */
int harmonize_gradients(
    const pulseq_SeqFile* seq,
    int* blockIDs,
    int n_blocks,
    int axis, /* 0=x, 1=y, 2=z */
    SegmentGradType* out_type,
    int* out_n_corners,
    int* out_n_points,
    float* out_raster_time
)
{
    int i;
    int grad_type;
    int is_trap, is_arb, is_etrap;
    int all_trap, all_same_timing, any_etrap, any_arb;
    float raster_time, max_raster;
    int n_corners, n_points;
    int channel;
    float delay, riseTime, flatTime, fallTime;
    pulseq_SeqBlock* block;
    pulseq_GradEvent* grad;
    pulseq_SeqBlock* prev_block;
    pulseq_GradEvent* prev_grad;

    all_trap = 1;
    all_same_timing = 1;
    any_etrap = 0;
    any_arb = 0;
    raster_time = GRAD_RASTER_US * 1e-6f;
    max_raster = raster_time;
    n_corners = 0;
    n_points = 0;
    channel = -1;

    /* First pass: check types and timing */
    for (i = 0; i < n_blocks; i++) {
        block = pulseq_getBlock(seq, blockIDs[i], 1);
        if (axis == 0) grad = &block->gx;
        else if (axis == 1) grad = &block->gy;
        else grad = &block->gz;

        grad_type = grad->type;
        is_trap = (grad_type == 1);
        is_arb = (grad_type == 2);
        is_etrap = (grad_type == 3);

        if (!is_trap) all_trap = 0;
        if (is_etrap) any_etrap = 1;
        if (is_arb) {
            any_arb = 1;
            if (grad->raster > max_raster) max_raster = grad->raster;
        }

        /* For traps, check timing consistency */
        if (is_trap && i > 0) {
            prev_block = pulseq_getBlock(seq, blockIDs[i-1], 1);
            if (axis == 0) prev_grad = &prev_block->gx;
            else if (axis == 1) prev_grad = &prev_block->gy;
            else prev_grad = &prev_block->gz;
            if (prev_grad->type == 1) {
                if (grad->delay != prev_grad->delay ||
                    grad->riseTime != prev_grad->riseTime ||
                    grad->flatTime != prev_grad->flatTime ||
                    grad->fallTime != prev_grad->fallTime) {
                    all_same_timing = 0;
                }
            }
        }
    }

    /* Decide output type */
    if (any_arb) {
        *out_type = SEG_ARB;
        *out_raster_time = max_raster;
        /* Find max n_points among ARBs */
        for (i = 0; i < n_blocks; i++) {
            block = pulseq_getBlock(seq, blockIDs[i], 1);
            if (axis == 0) grad = &block->gx;
            else if (axis == 1) grad = &block->gy;
            else grad = &block->gz;
            if (grad->type == 2 && grad->n_points > n_points) n_points = grad->n_points;
        }
        *out_n_points = n_points;
        *out_n_corners = 0;
    } else if (any_etrap || (all_trap && !all_same_timing)) {
        *out_type = SEG_ETRAP;
        *out_raster_time = raster_time;
        /* Count corners: union of all trap/etrap corners */
        for (i = 0; i < n_blocks; i++) {
            block = pulseq_getBlock(seq, blockIDs[i], 1);
            if (axis == 0) grad = &block->gx;
            else if (axis == 1) grad = &block->gy;
            else grad = &block->gz;
            n_corners += grad->n_corners; /* You may need to union actual corner positions */
        }
        *out_n_corners = n_corners;
        *out_n_points = 0;
    } else if (all_trap && all_same_timing) {
        *out_type = SEG_TRAP;
        *out_raster_time = raster_time;
        *out_n_corners = 4; /* Standard trapezoid */
        *out_n_points = 0;
    } else {
        /* Empty gradients: treat as zero-amplitude TRAP */
        *out_type = SEG_TRAP;
        *out_raster_time = raster_time;
        *out_n_corners = 4;
        *out_n_points = 0;
    }

    return 0;
}