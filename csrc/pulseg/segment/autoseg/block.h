/**
 * @file block.h
 * @brief Declarations for unique block and event identification in Pulseq sequences.
 */

#ifndef PULSEG_BLOCK_H
#define PULSEG_BLOCK_H

#include "../../pulseq.h"

/* Number of columns for block comparison */
#define BLOCK_COMPARE_COLS 20

/* Library size constants */
#define RF_LIBSIZE 10
#define GRAD_LIBSIZE 10
#define ADC_LIBSIZE 10
#define BLOCK_LIBSIZE 10

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
 * @brief Structure holding unique ADC identification results.
 */
typedef struct {
    float** unique_adcLibrary;    /**< Library of unique ADC events */
    int numUniqueADC;            /**< Number of unique ADC events */
    int* unique_adcID;           /**< Mapping from original ADC IDs to unique IDs */
} UniqueADCResult;

/**
 * @brief Structure holding complete unique sequence results.
 */
typedef struct {
    pulseq_SeqFile* unique_seq;   /**< New sequence with only unique events */
    BlockDefinition* uniqueBlocks; /**< Array of unique block definitions */
    int numUniqueBlocks;          /**< Number of unique blocks found */
    int* blockToUnique;           /**< Mapping from original blocks to unique blocks */
    int* unique_rfID;             /**< Mapping from original RF IDs to unique IDs */
    int* unique_gradID;           /**< Mapping from original gradient IDs to unique IDs */
    int* unique_adcID;            /**< Mapping from original ADC IDs to unique IDs */
} UniqueSeqResult;

/**
 * @brief Finds unique blocks in a Pulseq sequence file.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @return Pointer to UniqueBlocksResult containing unique blocks and mapping.
 */
UniqueBlocksResult* getUniqueBlocks(const pulseq_SeqFile* seq);

/**
 * @brief Gets unique blocks and sequence with only unique events.
 * 
 * @param[in] seq Original sequence file.
 * @return UniqueSeqResult containing unique blocks and sequence.
 */
UniqueSeqResult* getUniqueSeq(const pulseq_SeqFile* seq);

/**
 * @brief Public interface for unique ADC events.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @param[out] result Pointer to structure to store unique ADC results.
 */
void getUniqueADC(const pulseq_SeqFile* seq, UniqueADCResult* result);

/**
 * @brief Frees memory allocated for a UniqueBlocksResult structure.
 *
 * @param[in] result Pointer to UniqueBlocksResult to free.
 */
void freeUniqueBlocksResult(UniqueBlocksResult* result);

/**
 * @brief Frees memory allocated for a UniqueSeqResult structure.
 *
 * @param[in] result Pointer to UniqueSeqResult to free.
 */
void freeUniqueSeqResult(UniqueSeqResult* result);

#endif /* PULSEG_BLOCK_H */