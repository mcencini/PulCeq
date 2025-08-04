/**
 * @file segment_common.h
 * @brief Common structures for segmentation (autoseg and segparse).
 */

#ifndef SEGMENT_COMMON_H
#define SEGMENT_COMMON_H

#include <stddef.h>

#include "../../vendor.h"
#include "../../pulseq.h"

/* --- Unique Block Definition --- */
typedef struct {
    int duration;
    int rfID;
    int gxID;
    int gyID;
    int gzID;
    int adcID;
} BlockDefinition;

/* --- Unique Blocks Result --- */
typedef struct {
    BlockDefinition* uniqueBlocks;
    int numUniqueBlocks;
    int* blockToUnique; /* Array mapping each block to its unique block index */
} UniqueBlocksResult;

/* --- TR (periodic pattern) Definition --- */
typedef struct {
    int tr_id;      /* Unique identifier for the TR pattern */
    int n_blocks;   /* Number of blocks in this TR */
    int* blocks;    /* Array of block IDs in this TR */
} TRDefinition;

/* --- Sequence Patterns (TRs) --- */
typedef struct {
    int* tr_ids;            /* Array mapping each block to its TR ID */
    int* trid;              /* Array of size n_blocks, TR ID at start of each TR instance, 0 elsewhere */
    int n_blocks;           /* Total number of blocks */
    TRDefinition* tr_definitions; /* Array of TR definitions */
    int n_tr_definitions;   /* Number of TR definitions */
} SequencePatterns;

/* --- Segment Definition --- */
typedef struct {
    int* blockIDs;      /* Array of block IDs in the segment */
    int n_blocks;       /* Number of blocks in the segment */
    int nav_flag;       /* Navigation flag (from labelset.nav) */
} SegmentDefinition;

/* --- Segment Results --- */
typedef struct {
    SegmentDefinition* uniqueSegments;
    int n_uniqueSegments;
    int* blockToSegment; /* Array mapping each block to its segment ID */
} SegmentResults;

#endif /* SEGMENT_COMMON_H */