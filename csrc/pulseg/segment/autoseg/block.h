/**
 * @file block.h
 * @brief Unique block identification functions.
 */
#ifndef PULSEG_AUTOSEG_BLOCK_H
#define PULSEG_AUTOSEG_BLOCK_H

#include "../../../pulseq.h"

/**
 * @brief Get unique block IDs for all blocks in a sequence.
 * 
 * This function identifies unique blocks in the sequence based on their content
 * (RF, gradient, ADC events) and assigns unique IDs to them.
 *
 * @param seq Pointer to the Pulseq sequence
 * @return Array of block IDs or NULL if error
 */
int* getUniqueBlockIDs(pulseq_SeqFile* seq);

/**
 * @brief Free resources associated with unique block IDs.
 *
 * @param blockIds Array of block IDs to free
 */
void freeUniqueBlockIDs(int* blockIds);

#endif /* PULSEG_AUTOSEG_BLOCK_H */
