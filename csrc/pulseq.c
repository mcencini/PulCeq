/*
 * @file pulseq.c
 * @brief Implementation of public Pulseq API.
 */

#include "pulseq.h"

pulseq_SeqFile* pulseq_seqFile(char* filePath) { return __seqFile(filePath); }
void pulseq_seqFileFree(pulseq_SeqFile* seq) { __seqFileFree(seq); }
void pulseq_seqFileReset(pulseq_SeqFile* seq) { __seqFileReset(seq); }
void pulseq_readDefinitions(pulseq_SeqFile* seq) { __readDefinitions(seq); }
void pulseq_readLibraries(pulseq_SeqFile* seq, int readBlocks) { __readLibraries(seq, readBlocks); }
void pulseq_readSeq(pulseq_SeqFile* seq) { __readSeq(seq); }

pulseq_SeqBlock* pulseq_seqBlock(void) { return __seqBlock(); }
void pulseq_seqBlockFree(pulseq_SeqBlock* block) { __seqBlockFree(block); }
pulseq_SeqBlock* pulseq_getBlock(const pulseq_SeqFile* seq, int blockIndex, int parseExtensions) { return __getBlock(seq, blockIndex, parseExtensions); }
