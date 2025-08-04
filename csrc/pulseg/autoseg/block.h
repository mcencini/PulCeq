/**
 * @file block.h
 * @brief Declarations for unique block identification in Pulseq sequences.
 */

#ifndef PULSEG_BLOCK_H
#define PULSEG_BLOCK_H

#define BLOCK_COMPARE_COLS 20

/**
 * @brief Structure representing a block definition.
 */
typedef struct {
    int duration;
    int rfID;
    int gxID;
    int gyID;
    int gzID;
    int adcID;
} BlockDefinition;

/**
 * @brief Structure holding results of unique block identification.
 */
typedef struct {
    BlockDefinition* uniqueBlocks;
    int numUniqueBlocks;
    int* blockToUnique; /* length seq->numBlocks */
} UniqueBlocksResult;

/**
 * @brief Finds unique blocks in a Pulseq sequence file.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @return Pointer to UniqueBlocksResult containing unique blocks and mapping.
 */
UniqueBlocksResult* getUniqueBlocks(const pulseq_SeqFile* seq);
void freeUniqueBlocksResult(UniqueBlocksResult* result);

#endif /* PULSEG_BLOCK_H */