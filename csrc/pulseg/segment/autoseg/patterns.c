/**
 * @file patterns.c
 * @brief Implementation of TR pattern detection.
 */

#include <string.h>

#include "../../../vendor.h"

#include "patterns.h"

/**
 * @brief Find repeating pattern in block sequence.
 *
 * @param blockIds Array of block IDs
 * @param nBlocks Number of blocks
 * @param minLength Minimum pattern length
 * @param pattern Output array for pattern
 * @param maxPatternLength Maximum size of pattern array
 * @return Length of detected pattern or 0 if none found
 */
static int findPattern(const int* blockIds, int nBlocks, int minLength, int* pattern, int maxPatternLength)
{
    int i, j, k;
    int patternLength;
    int matches;
    
    /* Try patterns of increasing length */
    for (patternLength = minLength; patternLength <= nBlocks/2; patternLength++) {
        /* Check if this length divides sequence evenly */
        if (nBlocks % patternLength != 0) {
            continue;
        }
        
        /* Get first instance as potential pattern */
        for (i = 0; i < patternLength && i < maxPatternLength; i++) {
            pattern[i] = blockIds[i];
        }
        
        /* Check if pattern repeats */
        matches = 1;
        for (j = patternLength; j < nBlocks; j += patternLength) {
            for (k = 0; k < patternLength; k++) {
                if (blockIds[j+k] != pattern[k]) {
                    matches = 0;
                    break;
                }
            }
            if (!matches) break;
        }
        
        if (matches) {
            return patternLength;
        }
    }
    
    return 0;
}

SequencePatterns* detectSequencePatterns(
    const int* blockIds, 
    int nBlocks, 
    int minLength, 
    int tryReverse
)
{
    int i, j, k;
    int patternLength;
    int* pattern;
    int* reversePattern = NULL;
    int maxPatternLength;
    SequencePatterns* result;
    int trId;
    
    if (!blockIds || nBlocks <= 0 || minLength <= 0) {
        return NULL;
    }
    
    /* Allocate result structure */
    result = (SequencePatterns*)ALLOC(sizeof(SequencePatterns));
    if (!result) {
        return NULL;
    }
    memset(result, 0, sizeof(SequencePatterns));
    
    /* Allocate temporary buffers */
    maxPatternLength = nBlocks / 2;
    pattern = (int*)ALLOC(maxPatternLength * sizeof(int));
    if (!pattern) {
        FREE(result);
        return NULL;
    }
    
    if (tryReverse) {
        reversePattern = (int*)ALLOC(maxPatternLength * sizeof(int));
        if (!reversePattern) {
            FREE(pattern);
            FREE(result);
            return NULL;
        }
    }
    
    /* Find pattern */
    patternLength = findPattern(blockIds, nBlocks, minLength, pattern, maxPatternLength);
    
    if (patternLength == 0) {
        /* No pattern found */
        FREE(pattern);
        if (reversePattern) FREE(reversePattern);
        FREE(result);
        return NULL;
    }
    
    /* Allocate arrays in result */
    result->nBlocks = nBlocks;
    result->trIds = (int*)ALLOC(nBlocks * sizeof(int));
    result->trid = (int*)ALLOC(nBlocks * sizeof(int));
    if (!result->trIds || !result->trid) {
        FREE(pattern);
        if (reversePattern) FREE(reversePattern);
        FREE(result->trIds);
        FREE(result->trid);
        FREE(result);
        return NULL;
    }
    
    /* Fill TR IDs */
    memset(result->trid, 0, nBlocks * sizeof(int));
    for (i = 0; i < nBlocks; i += patternLength) {
        result->trid[i] = 1;  /* Mark start of each TR */
    }
    
    /* Create TR definition */
    result->nTrDefinitions = 1;
    result->trDefinitions = (TRDefinition*)ALLOC(sizeof(TRDefinition));
    if (!result->trDefinitions) {
        FREE(pattern);
        if (reversePattern) FREE(reversePattern);
        FREE(result->trIds);
        FREE(result->trid);
        FREE(result);
        return NULL;
    }
    
    /* Fill TR definition */
    trId = 1;
    result->trDefinitions[0].trId = trId;
    result->trDefinitions[0].nBlocks = patternLength;
    result->trDefinitions[0].blocks = (int*)ALLOC(patternLength * sizeof(int));
    if (!result->trDefinitions[0].blocks) {
        FREE(pattern);
        if (reversePattern) FREE(reversePattern);
        FREE(result->trIds);
        FREE(result->trid);
        FREE(result->trDefinitions);
        FREE(result);
        return NULL;
    }
    
    /* Copy pattern to TR definition */
    memcpy(result->trDefinitions[0].blocks, pattern, patternLength * sizeof(int));
    
    /* Set TR IDs for all blocks */
    for (i = 0; i < nBlocks; i++) {
        result->trIds[i] = trId;
    }
    
    /* Segments will be set by segmenter */
    result->trDefinitions[0].nSegments = 0;
    result->trDefinitions[0].segments = NULL;
    
    /* Free temporary buffers */
    FREE(pattern);
    if (reversePattern) FREE(reversePattern);
    
    return result;
}

void freeSequencePatterns(SequencePatterns* patterns)
{
    int i;
    
    if (!patterns) return;
    
    FREE(patterns->trIds);
    FREE(patterns->trid);
    
    if (patterns->trDefinitions) {
        for (i = 0; i < patterns->nTrDefinitions; i++) {
            FREE(patterns->trDefinitions[i].blocks);
            FREE(patterns->trDefinitions[i].segments);
        }
        FREE(patterns->trDefinitions);
    }
    
    FREE(patterns);
}
