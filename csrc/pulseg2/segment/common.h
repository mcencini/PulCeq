/**
 * @file common.h
 * @brief Common structures shared between segment detection methods.
 */
#ifndef PULSEG_SEGMENT_COMMON_H
#define PULSEG_SEGMENT_COMMON_H

#include <stddef.h>

#include "../../vendor.h"
#include "../../pulseq.h"

/* --- Block ID mapping --- */
typedef struct {
    int* blockToUnique;      /* Maps each block to its unique ID */
    int* blockToSegment;     /* Maps each block to its segment ID */
} BlockMapping;

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

/* --- Segment Results --- */
typedef struct {
    SegmentDefinition* uniqueSegments;
    int nUniqueSegments;
    int* blockToSegment; /* Array mapping each block to its segment ID */
} SegmentResults;

#endif /* PULSEG_SEGMENT_COMMON_H */