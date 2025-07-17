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

/** @struct SeqBlock
 * @brief  Sequence block containing RF, gradient, ADC, and trigger events.
 *
 *  @var SeqBlock::rf
 *    RF event.
 *  @var SeqBlock::gx
 *    Gradient event on X channel.
  *  @var SeqBlock::gy
 *    Gradient event on Y channel.
  *  @var SeqBlock::gz
 *    Gradient event on Z channel.
 *  @var SeqBlock::adc
 *    ADC event.
 *  @var SeqBlock::trigger
 *    Trigger event.
 *  @var SeqBlock::rotation
 *    Rotation event.
 *  @var SeqBlock::labelset
 *    Label set event.
 *  @var SeqBlock::labelinc
 *    Label increment event.
 *  @var SeqBlock::delay
 *    Soft delay event.
 *  @var SeqBlock::rfShimming
 *    RF shimming event.
 */
typedef struct {
    RFEvent rf;                 /**< RF event */
    GradEvent gx;               /**< Gradient event on X channel */
    GradEvent gy;               /**< Gradient event on Y channel */
    GradEvent gz;               /**< Gradient event on Z channel */
    ADCEvent adc;               /**< ADC event */
    TriggerEvent trigger;       /**< Trigger event */
    RotationEvent rotation;     /**< Rotation event */
    LabelEvent labelset;        /**< Label set event */
    LabelEvent labelinc;        /**< Label increment event */
    SoftDelayEvent delay;       /**< Soft delay event */
    RfShimmingEvent rfShimming; /**< RF shimming event */
} SeqBlock; /* Mirrors Pulseq SeqBlock */

/**
 * @brief Initializes a sequence block with default values.
 *
 */
SeqBlock* __seqBlock(void);

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
 * @return The SeqBlock corresponding to the specified index.
 */
SeqBlock* __getBlock(const SeqFile* seq, int blockIndex, int parseExtensions);

#endif /* BLOCK_H */