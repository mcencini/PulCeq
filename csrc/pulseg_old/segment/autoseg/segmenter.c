/**
 * @file segmenter.c
 * @brief Implementation of segment extraction.
 */

#include <string.h>

#include "../../../vendor.h"

#include "segmenter.h"

#define MAX_SEGMENTS_PER_TR 50
#define MIN_BLOCKS_PER_SEGMENT 1

/**
 * @brief Find segment boundaries within a TR.
 *
 * @param seq Pulseq sequence
 * @param trBlocks Array of block IDs in TR
 * @param nBlocks Number of blocks in TR
 * @param segmentBoundaries Output array for segment boundaries
 * @param maxBoundaries Maximum number of boundaries
 * @return Number of segment boundaries or -1 if error
 */
static int findSegmentBoundaries(
    const pulseq_SeqFile* seq,
    const int* trBlocks,
    int nBlocks,
    int* segmentBoundaries,
    int maxBoundaries
)
{
    int i, j;
    int nBoundaries = 0;
    float* block;
    
    /* First boundary is always at start */
    segmentBoundaries[nBoundaries++] = 0;
    
    /* Find ADC blocks - they always start a new segment */
    for (i = 1; i < nBlocks && nBoundaries < maxBoundaries; i++) {
        block = seq->blockLibrary[trBlocks[i]];
        
        /* Check if this block has ADC */
        if (block[5] > 0) { /* adcID > 0 */
            segmentBoundaries[nBoundaries++] = i;
        }
        
        /* TODO: Add more segmentation criteria */
    }
    
    /* Last boundary is always at end */
    if (nBoundaries < maxBoundaries) {
        segmentBoundaries[nBoundaries++] = nBlocks;
    }
    
    return nBoundaries;
}

/**
 * @brief Compare two segments for equality.
 *
 * @param seq Pulseq sequence
 * @param segment1 First segment's block IDs
 * @param nBlocks1 Number of blocks in first segment
 * @param segment2 Second segment's block IDs
 * @param nBlocks2 Number of blocks in second segment
 * @return 1 if segments are equal, 0 otherwise
 */
static int segmentsEqual(
    const pulseq_SeqFile* seq,
    const int* segment1,
    int nBlocks1,
    const int* segment2,
    int nBlocks2
)
{
    int i;
    
    if (nBlocks1 != nBlocks2) {
        return 0;
    }
    
    for (i = 0; i < nBlocks1; i++) {
        if (segment1[i] != segment2[i]) {
            return 0;
        }
    }
    
    return 1;
}

SegmentResults* extractSegments(
    const pulseq_SeqFile* seq, 
    const SequencePatterns* patterns
)
{
    int i, j, k, l;
    int trId, trLength;
    int nBoundaries;
    int segmentStart, segmentEnd, segmentLength;
    int segmentId;
    int found;
    int* segmentBoundaries;
    int* trBlocks;
    int* segmentBlocks;
    SegmentResults* results;
    TRDefinition* tr;
    int maxSegments;
    
    if (!seq || !patterns || patterns->nTrDefinitions <= 0) {
        return NULL;
    }
    
    /* Allocate result structure */
    results = (SegmentResults*)ALLOC(sizeof(SegmentResults));
    if (!results) {
        return NULL;
    }
    memset(results, 0, sizeof(SegmentResults));
    
    /* Allocate block to segment mapping */
    results->blockToSegment = (int*)ALLOC(seq->numBlocks * sizeof(int));
    if (!results->blockToSegment) {
        FREE(results);
        return NULL;
    }
    memset(results->blockToSegment, -1, seq->numBlocks * sizeof(int));
    
    /* Allocate temporary buffers */
    segmentBoundaries = (int*)ALLOC(MAX_SEGMENTS_PER_TR * sizeof(int));
    if (!segmentBoundaries) {
        FREE(results->blockToSegment);
        FREE(results);
        return NULL;
    }
    
    /* Initially allocate space for segments */
    maxSegments = patterns->nTrDefinitions * MAX_SEGMENTS_PER_TR;
    results->uniqueSegments = (SegmentDefinition*)ALLOC(maxSegments * sizeof(SegmentDefinition));
    if (!results->uniqueSegments) {
        FREE(segmentBoundaries);
        FREE(results->blockToSegment);
        FREE(results);
        return NULL;
    }
    memset(results->uniqueSegments, 0, maxSegments * sizeof(SegmentDefinition));
    results->nUniqueSegments = 0;
    
    /* Process each TR type */
    for (i = 0; i < patterns->nTrDefinitions; i++) {
        tr = &patterns->trDefinitions[i];
        trId = tr->trId;
        trLength = tr->nBlocks;
        
        /* Get blocks for this TR */
        trBlocks = tr->blocks;
        
        /* Find segment boundaries within this TR */
        nBoundaries = findSegmentBoundaries(seq, trBlocks, trLength, segmentBoundaries, MAX_SEGMENTS_PER_TR);
        if (nBoundaries < 2) {
            /* No segments found */
            continue;
        }
        
        /* Allocate segments array for this TR */
        tr->nSegments = nBoundaries - 1;
        tr->segments = (int*)ALLOC(tr->nSegments * sizeof(int));
        if (!tr->segments) {
            for (j = 0; j < results->nUniqueSegments; j++) {
                FREE(results->uniqueSegments[j].blockIDs);
            }
            FREE(results->uniqueSegments);
            FREE(segmentBoundaries);
            FREE(results->blockToSegment);
            FREE(results);
            return NULL;
        }
        
        /* Process segments */
        for (j = 0; j < nBoundaries - 1; j++) {
            segmentStart = segmentBoundaries[j];
            segmentEnd = segmentBoundaries[j+1];
            segmentLength = segmentEnd - segmentStart;
            
            /* Check if this segment already exists */
            found = 0;
            for (k = 0; k < results->nUniqueSegments; k++) {
                if (segmentsEqual(
                    seq, 
                    &trBlocks[segmentStart], segmentLength,
                    results->uniqueSegments[k].blockIDs, results->uniqueSegments[k].nBlocks
                )) {
                    found = 1;
                    segmentId = k;
                    break;
                }
            }
            
            /* If not found, create new segment */
            if (!found) {
                segmentId = results->nUniqueSegments;
                results->uniqueSegments[segmentId].nBlocks = segmentLength;
                results->uniqueSegments[segmentId].blockIDs = (int*)ALLOC(segmentLength * sizeof(int));
                if (!results->uniqueSegments[segmentId].blockIDs) {
                    for (k = 0; k < results->nUniqueSegments; k++) {
                        FREE(results->uniqueSegments[k].blockIDs);
                    }
                    FREE(results->uniqueSegments);
                    FREE(tr->segments);
                    FREE(segmentBoundaries);
                    FREE(results->blockToSegment);
                    FREE(results);
                    return NULL;
                }
                
                /* Copy block IDs */
                for (k = 0; k < segmentLength; k++) {
                    results->uniqueSegments[segmentId].blockIDs[k] = trBlocks[segmentStart + k];
                }
                
                results->uniqueSegments[segmentId].navFlag = 0; /* Default: not a navigator */
                results->nUniqueSegments++;
            }
            
            /* Add segment to TR */
            tr->segments[j] = segmentId;
        }
    }
    
    /* Map each block to its segment */
    /* Note: This is simplified and needs to be updated for actual implementation */
    for (i = 0; i < patterns->nBlocks; i++) {
        if (patterns->trid[i] > 0) {
            /* Start of TR */
            trId = patterns->trid[i];
            tr = &patterns->trDefinitions[trId-1]; /* Adjust for 1-based indexing */
            
            /* Map blocks in this TR to segments */
            for (j = 0; j < tr->nSegments; j++) {
                segmentId = tr->segments[j];
                segmentLength = results->uniqueSegments[segmentId].nBlocks;
                
                for (k = 0; k < segmentLength; k++) {
                    if (i + k < patterns->nBlocks) {
                        results->blockToSegment[i + k] = segmentId;
                    }
                }
                
                i += segmentLength - 1; /* Move to last block of segment */
            }
        }
    }
    
    /* Free temporary buffers */
    FREE(segmentBoundaries);
    
    return results;
}

void freeSegmentResults(SegmentResults* results)
{
    int i;
    
    if (!results) return;
    
    if (results->uniqueSegments) {
        for (i = 0; i < results->nUniqueSegments; i++) {
            FREE(results->uniqueSegments[i].blockIDs);
        }
        FREE(results->uniqueSegments);
    }
    
    FREE(results->blockToSegment);
    FREE(results);
}
