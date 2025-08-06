/**
 * @file pulseg.h
 * @brief Public API for PulSEG library.
 */

#ifndef PULSEG_H
#define PULSEG_H

#include "../pulseq.h"

#include "segment/common.h"

/**
 * @struct pulseg_Sequence
 * @brief Main sequence object with TR and segment information
 */
typedef struct {
    /* Sequence data */
    pulseq_SeqFile* seq;            /* Original Pulseq sequence */
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
    
    /* Processing flags */
    int isInitialized;              /* Whether sequence is initialized */
    int hasTridLabels;              /* Whether sequence has TRID labels */
} pulseg_Sequence;

/**
 * @brief Read a Pulseq sequence file and identify TRs and segments.
 *
 * @param filePath Path to the Pulseq .seq file
 * @param lazyParse If non-zero, only perform basic parsing without TR/segment analysis
 * @param uiParams Optional UI parameters for softDelay resolution (can be NULL)
 * @return Pointer to the parsed sequence or NULL if error
 */
pulseg_Sequence* pulseg_readSequence(const char* filePath, int lazyParse, void* uiParams);

/**
 * @brief Free all resources associated with a PulSEG sequence.
 *
 * @param pseq Pointer to the sequence to free
 */
void pulseg_freeSequence(pulseg_Sequence* pseq);

/**
 * @brief Get the current block based on iterator position.
 *
 * @param pseq Pointer to the sequence
 * @return Pointer to the current block or NULL if error
 */
pulseq_SeqBlock* pulseg_getBlock(pulseg_Sequence* pseq);

/**
 * @brief Move to the next block in the sequence.
 *
 * @param pseq Pointer to the sequence
 * @return 1 if successful, 0 if end of sequence, negative if error
 */
int pulseg_iterate(pulseg_Sequence* pseq);

/**
 * @brief Jump to a specific block (must be at TR boundary).
 *
 * @param pseq Pointer to the sequence
 * @param blockIndex Target block index
 * @return 0 if successful, negative if error
 */
int pulseg_gotoBlock(pulseg_Sequence* pseq, int blockIndex);

/**
 * @brief Initialize sequence iterator to beginning.
 *
 * @param pseq Pointer to the sequence
 * @return 0 if successful, negative if error
 */
int pulseg_initSequence(pulseg_Sequence* pseq);

#endif /* PULSEG_H */
