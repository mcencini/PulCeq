/**
 * @file segmenter.h
 * @brief Segment extraction and deduplication for Pulseq TRs.
 */

#ifndef PULSEG_SEGMENTER_H
#define PULSEG_SEGMENTER_H

#include "../../pulseq.h"

#include "patterns.h"

typedef struct {
    int* blockIDs;      /* Array of block IDs in the segment */
    int n_blocks;       /* Number of blocks in the segment */
    int nav_flag;       /* Navigation flag (from labelset.nav) */
} SegmentDefinition;

typedef struct {
    SegmentDefinition* uniqueSegments;
    int n_uniqueSegments;
    int* blockToSegment; /* Array mapping each block to its segment ID */
} SegmentResults;

/**
 * @brief Extracts and deduplicates segments from TR definitions.
 *
 * @param seq Pointer to parsed Pulseq sequence file.
 * @param patterns Pointer to SequencePatterns (TR definitions).
 * @return Pointer to SegmentResults structure.
 */
SegmentResults* extractSegments(const pulseq_SeqFile* seq, const SequencePatterns* patterns);

/**
 * @brief Frees memory allocated for SegmentResults.
 *
 * @param results Pointer to SegmentResults to free.
 */
void freeSegmentResults(SegmentResults* results);

#endif /* PULSEG_SEGMENTER_H */