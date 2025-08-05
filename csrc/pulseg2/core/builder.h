/**
 * @file builder.h
 * @brief Functions for building PulSEG sequence objects.
 */
#ifndef PULSEG_BUILDER_H
#define PULSEG_BUILDER_H

#include "../pulseg_internal.h"

/**
 * @brief Build a PulSEG sequence from segment analysis results.
 *
 * @param seq Original Pulseq sequence
 * @param patterns Detected sequence patterns (TR information)
 * @param segResults Extracted segments
 * @return Pointer to new PulSEG sequence object or NULL if error
 */
pulseg_Sequence* buildPulsegSequence(
    pulseq_SeqFile* seq,
    SequencePatterns* patterns,
    SegmentResults* segResults
);

/**
 * @brief Check if sequence contains TRID labels.
 *
 * @param seq Pulseq sequence to check
 * @return 1 if TRID labels are present, 0 otherwise
 */
int hasTridLabels(const pulseq_SeqFile* seq);

#endif /* PULSEG_BUILDER_H */