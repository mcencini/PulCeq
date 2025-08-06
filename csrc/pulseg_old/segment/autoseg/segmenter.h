/**
 * @file segmenter.h
 * @brief Segment extraction functions.
 */
#ifndef PULSEG_AUTOSEG_SEGMENTER_H
#define PULSEG_AUTOSEG_SEGMENTER_H

#include "../../../pulseq.h"

#include "../common.h"

#include "patterns.h"

/* --- Segment Results --- */
typedef struct {
    SegmentDefinition* uniqueSegments;
    int nUniqueSegments;
    int* blockToSegment; /* Array mapping each block to its segment ID */
} SegmentResults;

/**
 * @brief Extract segments from a sequence based on detected patterns.
 *
 * @param seq Pointer to the Pulseq sequence
 * @param patterns Detected sequence patterns
 * @return Extracted segments or NULL if error
 */
SegmentResults* extractSegments(
    const pulseq_SeqFile* seq, 
    const SequencePatterns* patterns
);

/**
 * @brief Free memory associated with segment results.
 *
 * @param results Pointer to segment results to free
 */
void freeSegmentResults(SegmentResults* results);

#endif /* PULSEG_AUTOSEG_SEGMENTER_H */
