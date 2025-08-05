/**
 * @file builder.c
 * @brief Implementation of PulSEG sequence builder.
 */

#include "builder.h"
#include "../../vendor.h"

pulseg_Sequence* buildPulsegSequence(
    pulseq_SeqFile* seq,
    SequencePatterns* patterns,
    SegmentResults* segResults
)
{
    pulseg_Sequence* result;
    
    /* Allocate result object */
    result = (pulseg_Sequence*)ALLOC(sizeof(pulseg_Sequence));
    if (!result) {
        return NULL;
    }
    
    /* Fill the result object */
    result->seq = seq;
    result->uniqueSeq = NULL; /* UniqueSeqResult is removed; we use seq directly */
    result->trid = patterns->trid;
    result->trDefs = patterns->trDefinitions;
    result->nTrDefs = patterns->nTrDefinitions;
    result->segments = segResults->uniqueSegments;
    result->nSegments = segResults->nUniqueSegments;
    result->blockToSegment = segResults->blockToSegment;
    
    /* Iterator state will be initialized by caller */
    
    return result;
}

int hasTridLabels(const pulseq_SeqFile* seq)
{
    int i;
    
    /* Check if TRID labels are defined in the sequence */
    if (!seq->isExtensionsLibraryParsed || seq->labelsetLibrarySize == 0) {
        return 0;
    }
    
    /* Check if any TRID label is used */
    for (i = 0; i < seq->labelsetLibrarySize; i++) {
        if (seq->labelsetLibrary[i][0] == TRID) { /* TRID defined in constants.h */
            return 1;
        }
    }
    
    return 0;
}