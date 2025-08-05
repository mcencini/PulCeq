/**
 * @file pulseg_internal.h
 * @brief Internal definitions for PulSEG implementation.
 */
#ifndef PULSEG_INTERNAL_H
#define PULSEG_INTERNAL_H

#include "../pulseq.h"

#include "segment/common.h"

/**
 * @brief Main PulSEG sequence structure.
 */
struct pulseg_Sequence {
    /* Sequence data */
    pulseq_SeqFile* seq;            /* Original Pulseq sequence */
    UniqueSeqResult* uniqueSeq;     /* Deduplicated sequence */
    int* trid;                      /* TR ID array (non-zero at TR starts) */
    TRDefinition* trDefs;           /* Array of TR definitions */
    int nTrDefs;                    /* Number of TR definitions */
    SegmentDefinition* segments;    /* Array of segment definitions */
    int nSegments;                  /* Number of segments */
    int* blockToSegment;            /* Map from block to segment ID */

    /* Iterator state */
    int currentBlock;               /* Current block index (n) */
    int currentTrId;                /* Current TR ID */
    int currentSegmentId;           /* Current segment ID */
    int currentBlockId;             /* Current unique block ID */
    int trCursor;                   /* Position within current TR */
    int segmentCursor;              /* Position within current segment */
};

#endif /* PULSEG_INTERNAL_H */