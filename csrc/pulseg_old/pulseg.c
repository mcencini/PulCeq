/**
 * @file pulseg.c
 * @brief Implementation of PulSEG public API.
 */
#include "pulseg.h"
#include "segment/autoseg/block.h"
#include "segment/autoseg/patterns.h"
#include "segment/autoseg/segmenter.h"
#include "../vendor.h"
#include <string.h>

/**
 * @brief Process soft delay events in a sequence.
 *
 * @param seq Pointer to the sequence
 * @param uiParams UI parameters for resolving soft delays
 * @return 0 if successful, non-zero if error
 */
static int processSoftDelays(pulseq_SeqFile* seq, void* uiParams)
{
    int i;
    int blockId;
    float actDelay;
    float* softDelay;
    
    if (!seq || !uiParams || seq->softDelayLibrarySize == 0) {
        return 0; /* Nothing to do */
    }
    
    /* Process each block */
    for (i = 0; i < seq->numBlocks; i++) {
        /* Check if block is a pure delay block */
        if (seq->blockLibrary[i][1] == 0 && /* rfID == 0 */
            seq->blockLibrary[i][2] == 0 && /* gxID == 0 */
            seq->blockLibrary[i][3] == 0 && /* gyID == 0 */
            seq->blockLibrary[i][4] == 0 && /* gzID == 0 */
            seq->blockLibrary[i][5] == 0 && /* adcID == 0 */
            seq->blockLibrary[i][6] > 0)    /* Has extension */
        {
            /* Look up soft delay extension - simplified, actual implementation would 
               parse the extension to find soft delay and compute actual duration */
            blockId = (int)seq->blockLibrary[i][6] - 1;
            softDelay = NULL;
            /* ... process soft delay ... */
            if (softDelay) {
                actDelay = softDelay[0]; /* + uiParam calculation based on softDelay[2] / softDelay[1] */
                seq->blockLibrary[i][0] = actDelay; /* Replace duration */
            }
        }
    }
    
    return 0;
}

/**
 * @brief Find segments in a sequence using automatic segmentation.
 *
 * @param pseq Pointer to the PulSEG sequence
 * @return 0 if successful, non-zero if error
 */
static int findSegments(pulseg_Sequence* pseq)
{
    int* blockIds;
    SequencePatterns* patterns;
    SegmentResults* segResults;
    int minTrLength = 4;
    int tryReverse = 1;
    
    /* Get unique block IDs */
    blockIds = getUniqueBlockIDs(pseq->seq);
    if (!blockIds) {
        return -1;
    }
    
    /* Detect TRs */
    patterns = detectSequencePatterns(blockIds, pseq->seq->numBlocks, minTrLength, tryReverse);
    if (!patterns) {
        freeUniqueBlockIDs(blockIds);
        return -2;
    }
    
    /* Extract segments */
    segResults = extractSegments(pseq->seq, patterns);
    if (!segResults) {
        freeSequencePatterns(patterns);
        freeUniqueBlockIDs(blockIds);
        return -3;
    }
    
    /* Store results in pseq */
    pseq->trid = patterns->trid;
    pseq->trDefs = patterns->trDefinitions;
    pseq->nTrDefs = patterns->nTrDefinitions;
    pseq->segments = segResults->uniqueSegments;
    pseq->nSegments = segResults->nUniqueSegments;
    pseq->blockToSegment = segResults->blockToSegment;
    
    /* Free intermediate resources (but not the data we're keeping) */
    FREE(patterns);
    FREE(segResults);
    freeUniqueBlockIDs(blockIds);
    
    return 0;
}

pulseg_Sequence* pulseg_readSequence(const char* filePath, int lazyParse, void* uiParams)
{
    pulseg_Sequence* pseq;
    
    /* 1. Create and initialize sequence object */
    pseq = (pulseg_Sequence*)ALLOC(sizeof(pulseg_Sequence));
    if (!pseq) return NULL;
    memset(pseq, 0, sizeof(pulseg_Sequence));
    
    /* 2. Read basic Pulseq file */
    pseq->seq = pulseq_createSeqFile();
    if (pulseq_read(pseq->seq, filePath) != 0) {
        pulseg_freeSequence(pseq);
        return NULL;
    }
    
    /* 3. Process softDelays if needed */
    if (pseq->seq->softDelayLibrarySize > 0 && uiParams != NULL) {
        if (processSoftDelays(pseq->seq, uiParams) != 0) {
            pulseg_freeSequence(pseq);
            return NULL;
        }
    }
    
    /* If only lazy parsing is needed, return here */
    if (lazyParse) {
        return pseq;
    }
    
    /* 4. Check for TRID labels */
    pseq->hasTridLabels = hasTridLabels(pseq->seq);
    
    /* 5. Process TR and segment information */
    if (pseq->hasTridLabels) {
        /* TODO: Use TRID labels for segmentation when implemented */
        /* parseTridSegments(pseq); */
        pulseg_freeSequence(pseq);
        return NULL;
    } else {
        /* Use automatic segmentation */
        if (findSegments(pseq) != 0) {
            pulseg_freeSequence(pseq);
            return NULL;
        }
    }
    
    /* 6. Initialize iterator */
    if (pulseg_initSequence(pseq) != 0) {
        pulseg_freeSequence(pseq);
        return NULL;
    }
    
    return pseq;
}

void pulseg_freeSequence(pulseg_Sequence* pseq)
{
    int i;
    
    if (!pseq) return;
    
    /* Free TR definitions */
    if (pseq->trDefs) {
        for (i = 0; i < pseq->nTrDefs; i++) {
            FREE(pseq->trDefs[i].blocks);
            FREE(pseq->trDefs[i].segments);
        }
        FREE(pseq->trDefs);
    }
    
    /* Free segment definitions */
    if (pseq->segments) {
        for (i = 0; i < pseq->nSegments; i++) {
            FREE(pseq->segments[i].blockIDs);
        }
        FREE(pseq->segments);
    }
    
    /* Free mapping arrays */
    FREE(pseq->trid);
    FREE(pseq->blockToSegment);
    
    /* Free original sequence */
    pulseq_freeSeqFile(pseq->seq);
    
    /* Free sequence object */
    FREE(pseq);
}
