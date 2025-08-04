/**
 * @file patterns.h
 * @brief Public API for periodic pattern detection in block sequences.
 */

#ifndef PULSEG_PATTERNS_H
#define PULSEG_PATTERNS_H

/**
 * @brief Structure representing a TR (periodic pattern) definition.
 */
typedef struct {
    int tr_id;      /**< Unique identifier for the TR pattern */
    int n_blocks;   /**< Number of blocks in this TR */
    int* blocks;    /**< Array of block IDs in this TR */
} TRDefinition;

/**
 * @brief Structure holding results of periodic pattern detection.
 */
typedef struct {
    int* tr_ids;            /**< Array mapping each block to its TR ID */
    int* trid;              /**< Array of size n_blocks, TR ID at start of each TR instance, 0 elsewhere */
    int n_blocks;           /**< Total number of blocks */
    TRDefinition* tr_definitions; /**< Array of TR definitions */
    int n_tr_definitions;   /**< Number of TR definitions */
} SequencePatterns;

/**
 * @brief Detects periodic patterns in block ID sequence.
 *
 * @param block_ids Array of block IDs.
 * @param n_blocks Number of blocks.
 * @param min_length Minimum pattern length.
 * @param try_reverse If nonzero, also try reversed sequence.
 * @return SequencePatterns structure with detected patterns.
 */
SequencePatterns detectSequencePatterns(const int* block_ids, int n_blocks, int min_length, int try_reverse);

/**
 * @brief Frees memory associated with SequencePatterns structure.
 *
 * @param patterns Pointer to SequencePatterns structure to free.
 */
void freeSequencePatterns(SequencePatterns* patterns);

#endif /* PULSEG_PATTERNS_H */