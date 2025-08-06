/**
 * @file pulseq.h
 * @brief Public API for Pulseq file parsing and block handling.
 */
#ifndef PULSEQ_H
#define PULSEQ_H

#include "pulseq/seqfile.h"
#include "pulseq/block.h"
#include "pulseq/event.h"

/* Type definitions for public API */
typedef LabelEvent pulseq_LabelEvent;

/* Public API */
int pulseq_seqFile(char* filePath, pulseq_SeqFile* seq);
void pulseq_seqFileFree(pulseq_SeqFile* seq);
void pulseq_seqFileReset(pulseq_SeqFile* seq);
void pulseq_readDefinitions(pulseq_SeqFile* seq);
void pulseq_readLibraries(pulseq_SeqFile* seq, int readBlocks);
void pulseq_readSeq(pulseq_SeqFile* seq);

int pulseq_seqBlock(pulseq_SeqBlock* block);
void pulseq_seqBlockFree(pulseq_SeqBlock* block);
int pulseq_getBlock(const pulseq_SeqFile* seq, int blockIndex, int parseExtensions, pulseq_SeqBlock* block);

/** @brief  Get label values for a specific ADC event.
 *  @param[in]  seq              Pointer to a SeqFile struct.
 *  @param[in]  adcIndex         Index of ADC event (0-based, in order of appearance in sequence).
 *  @param[out] labelOut         Pointer to a pre-allocated LabelEvent struct to fill with label values.
 *  @returns    int              1 if successful, 0 if labels couldn't be retrieved.
 */
int pulseq_getLabelsForAdc(const pulseq_SeqFile* seq, int adcIndex, pulseq_LabelEvent* labelOut);

#endif /* PULSEQ_H */
