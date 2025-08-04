/**
 * @file segmenter.h
 * @brief Segment extraction and deduplication for Pulseq TRs.
 */

#ifndef PULSEG_SEGMENTER_H
#define PULSEG_SEGMENTER_H

#include "../common.h"

SegmentResults* extractSegments(const pulseq_SeqFile* seq, const SequencePatterns* patterns);
void freeSegmentResults(SegmentResults* results);

#endif /* PULSEG_SEGMENTER_H */