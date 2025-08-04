/**
 * @file pulseq.h
 * @brief Public API for Pulseq file parsing and block handling.
 */
#ifndef PULSEQ_H
#define PULSEQ_H

#include "pulseq/seqfile.h"
#include "pulseq/block.h"

/* Public API */
pulseq_SeqFile* pulseq_seqFile(char* filePath);
void pulseq_seqFileFree(pulseq_SeqFile* seq);
void pulseq_seqFileReset(pulseq_SeqFile* seq);
void pulseq_readDefinitions(pulseq_SeqFile* seq);
void pulseq_readLibraries(pulseq_SeqFile* seq, int readBlocks);
void pulseq_readSeq(pulseq_SeqFile* seq);

pulseq_SeqBlock* pulseq_seqBlock(void);
void pulseq_seqBlockFree(pulseq_SeqBlock* block);
pulseq_SeqBlock* pulseq_getBlock(const pulseq_SeqFile* seq, int blockIndex, int parseExtensions);

#endif /* PULSEQ_H */
