/**
 * @file pulseq.h
 * @brief Public API for Pulseq file parsing and block handling.
 */
#ifndef PULSEQ_H
#define PULSEQ_H

#include "pulseq/block.h"
#include "pulseq/seqfile.h"

/* Public API */
SeqFile* seqFile(char* filePath);
void seqFileFree(SeqFile* seq);
void seqFileReset(SeqFile* seq);
void readDefinitions(SeqFile* seq);
void readLibraries(SeqFile* seq, int readBlocks);
void readSeq(SeqFile* seq);

SeqBlock* seqBlock(void);
void seqBlockFree(SeqBlock* block);
SeqBlock* getBlock(const SeqFile* seq, int blockIndex, int parseExtensions);

#endif /* PULSEQ_H */
