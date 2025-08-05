/**
 * @file pulseg.c
 * @brief Main entry point for PulSEG library.
 */

#include "pulseg_internal.h"
#include "segment/autoseg/block.h"
#include "segment/autoseg/patterns.h"
#include "segment/autoseg/segmenter.h"

pulseg_Sequence* pulseg_readSequence(const char* filePath)
{
    pulseq_SeqFile* seq;
    UniqueSeqResult* uniqueSeqResult;
    SequencePatterns* patterns;
    SegmentResults* segResults;
    pulseg_Sequence* result;
    int hasTridLabels;
    int minTrLength = 4; /* Minimum TR length for detection */
    int tryReverse = 1;  /* Try reverse patterns */

    /* 1. Read basic Pulseq file */
    seq = pulseq_createSeqFile();
    if (pulseq_read(seq, filePath) != 0) return NULL;
    
    /* 2. Find unique blocks */
    uniqueSeqResult = getUniqueSeq(seq);
    if (!uniqueSeqResult) {
        pulseq_freeSeqFile(seq);
        return NULL;
    }
    
    /* 3. Detect TRs (either from TRID labels or autoseg) */
    hasTridLabels = 0; /* TODO: Implement check for TRID labels */
    
    if (hasTridLabels) {
        /* TODO: Use segparse when implemented */
        patterns = NULL; /* parseTRIDLabels(seq); */
    } else {
        /* Autoseg pattern detection */
        patterns = detectSequencePatterns(uniqueSeqResult->blockToUnique, seq->numBlocks, minTrLength, tryReverse);
    }
    
    if (!patterns) {
        freeUniqueSeqResult(uniqueSeqResult);
        pulseq_freeSeqFile(seq);
        return NULL;
    }
    
    /* 4. Extract segments */
    segResults = extractSegments(seq, patterns);
    if (!segResults) {
        freeSequencePatterns(patterns);
        freeUniqueSeqResult(uniqueSeqResult);
        pulseq_freeSeqFile(seq);
        return NULL;
    }
    
    /* 5. Build result object */
    result = (pulseg_Sequence*)ALLOC(sizeof(pulseg_Sequence));
    if (!result) {
        freeSegmentResults(segResults);
        freeSequencePatterns(patterns);
        freeUniqueSeqResult(uniqueSeqResult);
        pulseq_freeSeqFile(seq);
        return NULL;
    }
    
    /* Fill the result object */
    result->seq = seq;
    result->uniqueSeq = uniqueSeqResult;
    result->trid = patterns->trid;
    result->trDefs = patterns->trDefinitions;
    result->nTrDefs = patterns->nTrDefinitions;
    result->segments = segResults->uniqueSegments;
    result->nSegments = segResults->nUniqueSegments;
    result->blockToSegment = segResults->blockToSegment;
    
    /* Initialize iterator state */
    pulseg_initSequence(result);
    
    /* Free intermediate results (but not the data we keep) */
    FREE(patterns); /* Only free the container, not the data */
    FREE(segResults); /* Only free the container, not the data */
    
    return result;
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
    
    /* Free unique seq result */
    freeUniqueSeqResult(pseq->uniqueSeq);
    
    /* Free original sequence */
    pulseq_freeSeqFile(pseq->seq);
    
    /* Free sequence object */
    FREE(pseq);
}