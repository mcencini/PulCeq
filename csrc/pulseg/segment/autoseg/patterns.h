/**
 * @file patterns.h
 * @brief TR pattern detection functions.
 */
#ifndef PULSEG_AUTOSEG_PATTERNS_H
#define PULSEG_AUTOSEG_PATTERNS_H

#include "../common.h"

/* --- Sequence Patterns (TRs) --- */
typedef struct {
    int* trIds;            /* Array mapping each block to its TR ID */
    int* trid;             /* Array of size nBlocks, TR ID at start of each TR instance, 0 elsewhere */
    int nBlocks;           /* Total number of blocks */
    TRDefinition* trDefinitions; /* Array of TR definitions */
    int nTrDefinitions;    /* Number of TR definitions */
} SequencePatterns;

/**
 * @brief Detect periodic patterns (TRs) in a sequence of block IDs.
 *
 * @param blockIds Array of block IDs
 * @param nBlocks Number of blocks in the array
 * @param minLength Minimum length of a pattern to detect
 * @param tryReverse Whether to try detecting reverse patterns
 * @return Detected sequence patterns or NULL if error
 */
SequencePatterns* detectSequencePatterns(
    const int* blockIds, 
    int nBlocks, 
    int minLength, 
    int tryReverse
);

/**
 * @brief Free memory associated with sequence patterns.
 *
 * @param patterns Pointer to sequence patterns to free
 */
void freeSequencePatterns(SequencePatterns* patterns);

#endif /* PULSEG_AUTOSEG_PATTERNS_H */
