/**
 * @file common.c
 * @brief Implementation of common segment functions.
 */

#include "common.h"

/* Common utility functions shared between segment detection methods */

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

/* Other common functions can be added here */