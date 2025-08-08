#ifndef UNIQUE_SEQUENCE_H
#define UNIQUE_SEQUENCE_H

#include "seqfile.h"

/**
 * @brief Create a new sequence with unique RF, gradient, ADC events, and blocks.
 * 
 * This function deduplicates the input sequence and generates a unique sequence
 * containing only the necessary libraries and mappings.
 * 
 * @param[out] uniqueSeq Pointer to the output SeqFile structure for the unique sequence.
 * @param[in] seq Pointer to the input sequence file.
 * @return int 1 if successful, 0 if failed.
 */
int getUniqueSeq(SeqFile* uniqueSeq, const SeqFile* seq);

#endif /* UNIQUE_SEQUENCE_H */