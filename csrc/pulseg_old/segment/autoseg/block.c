/**
 * @file block.c
 * @brief Implementation of unique block identification.
 */

#include <string.h>

#include "../../../vendor.h"

#include "block.h"

#define BLOCK_COMPARE_COLS 20  /* Number of columns to compare in block library */

/**
 * @brief Compare two blocks for equality.
 *
 * @param seq Pulseq sequence
 * @param blockId1 First block ID
 * @param blockId2 Second block ID
 * @return 1 if blocks are equal, 0 otherwise
 */
static int blocksEqual(pulseq_SeqFile* seq, int blockId1, int blockId2)
{
    int i;
    float *block1, *block2;
    
    block1 = seq->blockLibrary[blockId1];
    block2 = seq->blockLibrary[blockId2];
    
    /* Compare block properties */
    for (i = 0; i < BLOCK_COMPARE_COLS; i++) {
        if (block1[i] != block2[i]) {
            return 0;
        }
    }
    
    /* TODO: Compare extensions if needed */
    
    return 1;
}

int* getUniqueBlockIDs(pulseq_SeqFile* seq)
{
    int i, j, uniqueId;
    int* blockIds;
    
    if (!seq || seq->numBlocks <= 0) {
        return NULL;
    }
    
    /* Allocate array for block IDs */
    blockIds = (int*)ALLOC(seq->numBlocks * sizeof(int));
    if (!blockIds) {
        return NULL;
    }
    
    /* First block is always unique */
    blockIds[0] = 0;
    uniqueId = 1;
    
    /* Process remaining blocks */
    for (i = 1; i < seq->numBlocks; i++) {
        /* Check if this block matches any previous unique block */
        for (j = 0; j < i; j++) {
            if (blocksEqual(seq, i, j)) {
                blockIds[i] = blockIds[j];
                break;
            }
        }
        
        /* If no match found, assign new unique ID */
        if (j == i) {
            blockIds[i] = uniqueId++;
        }
    }
    
    return blockIds;
}

void freeUniqueBlockIDs(int* blockIds)
{
    FREE(blockIds);
}
