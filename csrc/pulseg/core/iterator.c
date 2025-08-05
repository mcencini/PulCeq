/**
 * @file iterator.c
 * @brief Implementation of sequence iterator functions.
 */

#include "../pulseg.h"
#include "../../vendor.h"

pulseq_SeqBlock* pulseg_getBlock(pulseg_Sequence* pseq)
{
    /* Use stored currentBlock to fetch from original sequence */
    return pulseq_getBlock(pseq->seq, pseq->currentBlock, 1);
}

int pulseg_iterate(pulseg_Sequence* pseq)
{
    int segmentId;
    SegmentDefinition* segment;
    TRDefinition* tr;
    int nextTrId;

    /* Safety check */
    if (pseq->currentBlock >= pseq->seq->numBlocks - 1) {
        return 0; /* End of sequence */
    }

    /* Move to next block */
    pseq->currentBlock++;
    
    /* Get current segment */
    segmentId = pseq->blockToSegment[pseq->currentBlock];
    segment = &pseq->segments[segmentId];
    
    /* Update segment cursor */
    pseq->segmentCursor++;
    
    /* If we've reached the end of the segment */
    if (pseq->segmentCursor >= segment->nBlocks) {
        /* Move to next segment */
        pseq->trCursor++;
        tr = &pseq->trDefs[pseq->currentTrId];
        
        /* If we've reached the end of the TR */
        if (pseq->trCursor >= tr->nSegments) {
            /* Look for next TR start */
            nextTrId = pseq->trid[pseq->currentBlock];
            if (nextTrId == 0) {
                /* Not at TR boundary - error state */
                return -1;
            }
            
            /* Update to new TR */
            pseq->currentTrId = nextTrId;
            pseq->trCursor = 0;
            
            /* Get first segment of new TR */
            tr = &pseq->trDefs[nextTrId];
            pseq->currentSegmentId = tr->segments[0];
        }
        else {
            /* Get next segment in current TR */
            pseq->currentSegmentId = tr->segments[pseq->trCursor];
        }
        
        /* Reset segment cursor and get first block */
        pseq->segmentCursor = 0;
        segment = &pseq->segments[pseq->currentSegmentId];
    }
    
    /* Update current block ID */
    pseq->currentBlockId = segment->blockIDs[pseq->segmentCursor];
    
    return 1; /* Successful iteration */
}

int pulseg_gotoBlock(pulseg_Sequence* pseq, int blockIndex)
{
    TRDefinition* tr;
    SegmentDefinition* segment;
    
    /* Validate index */
    if (blockIndex < 0 || blockIndex >= pseq->seq->numBlocks) {
        return -1; /* Invalid index */
    }
    
    /* Must be at TR boundary */
    if (blockIndex > 0 && pseq->trid[blockIndex] == 0) {
        return -2; /* Not at TR boundary */
    }
    
    /* Set block index */
    pseq->currentBlock = blockIndex;
    
    /* Get TR ID and reset TR cursor */
    pseq->currentTrId = pseq->trid[blockIndex];
    pseq->trCursor = 0;
    
    /* Get first segment in TR */
    tr = &pseq->trDefs[pseq->currentTrId];
    pseq->currentSegmentId = tr->segments[0];
    
    /* Reset segment cursor */
    pseq->segmentCursor = 0;
    
    /* Get first block in segment */
    segment = &pseq->segments[pseq->currentSegmentId];
    pseq->currentBlockId = segment->blockIDs[0];
    
    return 0; /* Success */
}

int pulseg_initSequence(pulseg_Sequence* pseq)
{
    TRDefinition* tr;
    SegmentDefinition* segment;
    
    /* Start at first block */
    pseq->currentBlock = 0;
    
    /* Get first TR */
    pseq->currentTrId = pseq->trid[0];
    pseq->trCursor = 0;
    
    /* Get first segment */
    tr = &pseq->trDefs[pseq->currentTrId];
    pseq->currentSegmentId = tr->segments[0];
    pseq->segmentCursor = 0;
    
    /* Get first block */
    segment = &pseq->segments[pseq->currentSegmentId];
    pseq->currentBlockId = segment->blockIDs[0];
    
    pseq->isInitialized = 1;
    return 0;
}
