/**
 * @file patterns.h
 * @brief Public API for periodic pattern detection in block sequences.
 */

#ifndef PULSEG_PATTERNS_H
#define PULSEG_PATTERNS_H

#include "../common.h"

SequencePatterns detectSequencePatterns(const int* block_ids, int n_blocks, int min_length, int try_reverse);
void freeSequencePatterns(SequencePatterns* patterns);

#endif /* PULSEG_PATTERNS_H */