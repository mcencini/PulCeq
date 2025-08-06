/**
 * @file block.h
 * @brief Public API for defining and manipulating Pulseq blocks.
 *
 */
#ifndef BLOCK_H
#define BLOCK_H

#include "event.h"

#ifndef SEQFILE_H
typedef struct SeqFile SeqFile; /* Forward declaration to avoid circular dependency */
#endif /* SEQFILE_H */

#define MAX_EXTENSIONS_PER_BLOCK 64

/**
 * @struct RawBlock
 * @brief  Raw block content IDs and extension data.
 *
 * This structure holds the content IDs of a block and its extensions.
 * It is used to retrieve the raw data from the sequence file.
 */
typedef struct {
    int block_duration;
    int rf;
    int gx;
    int gy;
    int gz;
    int adc;
    int extCount;
    int ext[MAX_EXTENSIONS_PER_BLOCK][2];  /* [type, ref] */
} RawBlock;

/**
 * @brief Get the raw block content IDs from the sequence file.
 *
 * @param[in, out] block Pointer to the block's content IDs and extension data.
 * @param[in] seq Pointer to the SeqFile structure.
 * @param[in] blockIndex Index of the block to retrieve.
 * @param[in] parseExtensions Flag indicating whether to parse extensions.
 */
void getRawBlockContentIDs(RawBlock* block, const SeqFile* seq, const int blockIndex, const int parseExtensions);

/** @struct pulseq_SeqBlock
 * @brief  Sequence block containing RF, gradient, ADC, and trigger events.
 *
 *  @var pulseq_SeqBlock::duration
 *    Duration of the block in microseconds.
 *  @var pulseq_SeqBlock::rf
 *    RF event.
 *  @var pulseq_SeqBlock::gx
 *    Gradient event on X channel.
  *  @var pulseq_SeqBlock::gy
 *    Gradient event on Y channel.
  *  @var pulseq_SeqBlock::gz
 *    Gradient event on Z channel.
 *  @var pulseq_SeqBlock::adc
 *    ADC event.
 *  @var pulseq_SeqBlock::trigger
 *    Trigger event.
 *  @var pulseq_SeqBlock::rotation
 *    Rotation event.
 *  @var pulseq_SeqBlock::labelset
 *    Label set event.
 *  @var pulseq_SeqBlock::labelinc
 *    Label increment event.
 *  @var pulseq_SeqBlock::delay
 *    Soft delay event.
 *  @var pulseq_SeqBlock::rfShimming
 *    RF shimming event.
 */
typedef struct {
    int duration;               /**< @brief Duration of the block (us) */
    RFEvent rf;                 /**< @brief RF event */
    GradEvent gx;               /**< @brief Gradient event on X channel */
    GradEvent gy;               /**< @brief Gradient event on Y channel */
    GradEvent gz;               /**< @brief Gradient event on Z channel */
    ADCEvent adc;               /**< @brief ADC event */
    TriggerEvent trigger;       /**< @brief Trigger event */
    RotationEvent rotation;     /**< @brief Rotation event */
    FlagEvent flag;             /**< @brief Flag event containing flag values */
    LabelEvent label;           /**< @brief Label event containing the actual label values */
    SoftDelayEvent delay;       /**< @brief Soft delay event */
    RfShimmingEvent rfShimming; /**< @brief RF shimming event */
} pulseq_SeqBlock; /* Mirrors Pulseq SeqBlock */

typedef pulseq_SeqBlock SeqBlock;

/**
 * @brief Initializes a sequence block with default values.
 *
 * @param[out] block The pre-allocated block structure to initialize
 * @return 1 if successful, 0 if failed
 */
int __seqBlock(SeqBlock* block);

/**
 * @brief Frees all resources associated with a SeqBlock.
 *
 * This function deallocates memory for all waveform samples and resets the block.
 *
 * @param[in,out] block The SeqBlock to be freed.
 */
void __seqBlockFree(SeqBlock* block);

/**
 * @brief Retrieves a block from the sequence file.
 *
 * @param[in] seq The SeqFile structure containing the sequence data.
 * @param[in] blockIndex The index of the block to retrieve.
 * @param[in] parseExtensions Flag indicating whether to parse extensions.
 * @param[out] block Pointer to a pre-allocated SeqBlock to fill.
 * @return 1 if successful, 0 otherwise.
 */
int __getBlock(const SeqFile* seq, int blockIndex, int parseExtensions, SeqBlock* block);

#endif /* BLOCK_H */