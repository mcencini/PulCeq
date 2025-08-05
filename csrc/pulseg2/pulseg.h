/**
 * @file pulseg.h
 * @brief Public API for PulSEG library.
 */
#ifndef PULSEG_H
#define PULSEG_H

#include "../pulseq.h"

/* Main sequence object */
typedef struct pulseg_Sequence pulseg_Sequence;

/**
 * @brief Read and process a Pulseq file into a PulSEG sequence.
 * 
 * @param filePath Path to the Pulseq .seq file
 * @return Pointer to processed sequence or NULL if error
 */
pulseg_Sequence* pulseg_readSequence(const char* filePath);

/**
 * @brief Free a PulSEG sequence and all associated memory.
 * 
 * @param pseq Pointer to sequence to free
 */
void pulseg_freeSequence(pulseg_Sequence* pseq);

/**
 * @brief Get current block from sequence iterator.
 * 
 * @param pseq Pointer to sequence
 * @return Current block or NULL if error
 */
pulseq_SeqBlock* pulseg_getBlock(pulseg_Sequence* pseq);

/**
 * @brief Move to next block in sequence.
 * 
 * @param pseq Pointer to sequence
 * @return 1 if successful, 0 if end of sequence, negative if error
 */
int pulseg_iterate(pulseg_Sequence* pseq);

/**
 * @brief Jump to specific block (must be at TR boundary).
 * 
 * @param pseq Pointer to sequence
 * @param blockIndex Target block index
 * @return 0 if successful, negative if error
 */
int pulseg_gotoBlock(pulseg_Sequence* pseq, int blockIndex);

/**
 * @brief Initialize sequence iterator to beginning.
 * 
 * @param pseq Pointer to sequence
 * @return 0 if successful, negative if error
 */
int pulseg_initSequence(pulseg_Sequence* pseq);

#endif /* PULSEG_H */