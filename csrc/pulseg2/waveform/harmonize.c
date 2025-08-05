/**
 * @file harmonize.c
 * @brief Implementation of gradient harmonization functions.
 */

#include "../segment/common.h"

int harmonizeGradients(
    const pulseq_SeqFile* seq,
    int* blockIDs,
    int nBlocks,
    int axis,
    SegmentGradType* outType,
    int* outNCorners,
    int* outNPoints,
    float* outRasterTime
)
{
    int i;
    int gradType;
    int isTrap, isArb, isEtrap;
    int allTrap, allSameTiming, anyEtrap, anyArb;
    float rasterTime, maxRaster;
    int nCorners, nPoints;
    int channel;
    float delay, riseTime, flatTime, fallTime;
    pulseq_SeqBlock* block;
    pulseq_GradEvent* grad;
    pulseq_SeqBlock* prevBlock;
    pulseq_GradEvent* prevGrad;

    allTrap = 1;
    allSameTiming = 1;
    anyEtrap = 0;
    anyArb = 0;
    rasterTime = GRAD_RASTER_US * 1e-6f;
    maxRaster = rasterTime;
    nCorners = 0;
    nPoints = 0;
    channel = -1;

    /* First pass: check types and timing */
    for (i = 0; i < nBlocks; i++) {
        block = pulseq_getBlock(seq, blockIDs[i], 1);
        if (axis == 0) grad = &block->gx;
        else if (axis == 1) grad = &block->gy;
        else grad = &block->gz;

        gradType = grad->type;
        isTrap = (gradType == 1);
        isArb = (gradType == 2);
        isEtrap = (gradType == 3);

        if (!isTrap) allTrap = 0;
        if (isEtrap) anyEtrap = 1;
        if (isArb) {
            anyArb = 1;
            if (grad->raster > maxRaster) maxRaster = grad->raster;
        }

        /* For traps, check timing consistency */
        if (isTrap && i > 0) {
            prevBlock = pulseq_getBlock(seq, blockIDs[i-1], 1);
            if (axis == 0) prevGrad = &prevBlock->gx;
            else if (axis == 1) prevGrad = &prevBlock->gy;
            else prevGrad = &prevBlock->gz;
            if (prevGrad->type == 1) {
                if (grad->delay != prevGrad->delay ||
                    grad->riseTime != prevGrad->riseTime ||
                    grad->flatTime != prevGrad->flatTime ||
                    grad->fallTime != prevGrad->fallTime) {
                    allSameTiming = 0;
                }
            }
        }
    }

    /* Decide output type */
    if (anyArb) {
        *outType = SEG_ARB;
        *outRasterTime = maxRaster;
        /* Find max n_points among ARBs */
        for (i = 0; i < nBlocks; i++) {
            block = pulseq_getBlock(seq, blockIDs[i], 1);
            if (axis == 0) grad = &block->gx;
            else if (axis == 1) grad = &block->gy;
            else grad = &block->gz;
            if (grad->type == 2 && grad->nPoints > nPoints) nPoints = grad->nPoints;
        }
        *outNPoints = nPoints;
        *outNCorners = 0;
    } else if (anyEtrap || (allTrap && !allSameTiming)) {
        *outType = SEG_ETRAP;
        *outRasterTime = rasterTime;
        /* Count corners: union of all trap/etrap corners */
        for (i = 0; i < nBlocks; i++) {
            block = pulseq_getBlock(seq, blockIDs[i], 1);
            if (axis == 0) grad = &block->gx;
            else if (axis == 1) grad = &block->gy;
            else grad = &block->gz;
            nCorners += grad->nCorners; /* You may need to union actual corner positions */
        }
        *outNCorners = nCorners;
        *outNPoints = 0;
    } else if (allTrap && allSameTiming) {
        *outType = SEG_TRAP;
        *outRasterTime = rasterTime;
        *outNCorners = 4; /* Standard trapezoid */
        *outNPoints = 0;
    } else {
        /* Empty gradients: treat as zero-amplitude TRAP */
        *outType = SEG_TRAP;
        *outRasterTime = rasterTime;
        *outNCorners = 4;
        *outNPoints = 0;
    }

    return 0;
}