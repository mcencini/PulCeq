/**
 * @file pulseq.h
 * @brief Public API for Pulseq file parsing and block handling.
 */
#ifndef PULSEQ_H
#define PULSEQ_H

#include "pulseq/seqfile.h"
#include "pulseq/block.h"
#include "pulseq/event.h"
#include "pulseq/label.h"
#include "pulseq/unique_sequence.h"
#include "pulseq/check_sequence.h"
#include "pulseq/ui_config.h"

/* Type definitions for public API */
typedef LabelEvent pulseq_LabelEvent;
typedef LabelLimits pulseq_LabelLimits;
typedef UIConfig pulseq_UIConfig;

/* Public API */
int pulseq_seqFile(char* filePath, pulseq_SeqFile* seq);
void pulseq_seqFileFree(pulseq_SeqFile* seq);
void pulseq_seqFileReset(pulseq_SeqFile* seq);
void pulseq_readDefinitions(pulseq_SeqFile* seq);
void pulseq_readLibraries(pulseq_SeqFile* seq, int readBlocks);
void pulseq_readSeq(pulseq_SeqFile* seq, int forceDecompression);

int pulseq_seqBlock(pulseq_SeqBlock* block);
void pulseq_seqBlockFree(pulseq_SeqBlock* block);
int pulseq_getBlock(const pulseq_SeqFile* seq, int blockIndex, int parseExtensions, pulseq_SeqBlock* block);

int pulseq_getUniqueSeq(pulseq_SeqFile* uniqueSeq, const pulseq_SeqFile* seq);

#endif /* PULSEQ_H */
