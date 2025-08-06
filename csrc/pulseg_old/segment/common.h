/**
 * @file common.h
 * @brief Common structures shared between segment detection methods.
 */
#ifndef PULSEG_SEGMENT_COMMON_H
#define PULSEG_SEGMENT_COMMON_H

#include "../../pulseq.h"

/* --- TR (periodic pattern) Definition --- */
typedef struct {
    int trId;       /* Unique identifier for the TR pattern */
    int nBlocks;    /* Number of blocks in this TR */
    int* blocks;    /* Array of block IDs in this TR */
    int nSegments;  /* Number of segments in this TR */
    int* segments;  /* Array of segment IDs in this TR */
} TRDefinition;

/* --- Segment Definition --- */
typedef struct {
    int* blockIDs;      /* Array of block IDs in the segment */
    int nBlocks;        /* Number of blocks in the segment */
    int navFlag;        /* Navigation flag (from labelset.nav) */
} SegmentDefinition;

/**
 * @brief Check if sequence contains TRID labels.
 *
 * @param seq Pulseq sequence to check
 * @return 1 if TRID labels are present, 0 otherwise
 */
int hasTridLabels(const pulseq_SeqFile* seq);

#endif /* PULSEG_SEGMENT_COMMON_H */
