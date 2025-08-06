/*
 * @file pulseq.c
 * @brief Implementation of public Pulseq API.
 */

#include "pulseq.h"

int pulseq_seqFile(char* filePath, pulseq_SeqFile* seq) { return __seqFile(filePath, seq); }
void pulseq_seqFileFree(pulseq_SeqFile* seq) { __seqFileFree(seq); }
void pulseq_seqFileReset(pulseq_SeqFile* seq) { __seqFileReset(seq); }
void pulseq_readDefinitions(pulseq_SeqFile* seq) { __readDefinitions(seq); }
void pulseq_readLibraries(pulseq_SeqFile* seq, int readBlocks) { __readLibraries(seq, readBlocks); }
void pulseq_readSeq(pulseq_SeqFile* seq) { __readSeq(seq); }

int pulseq_seqBlock(pulseq_SeqBlock* block) { return __seqBlock(block); }
void pulseq_seqBlockFree(pulseq_SeqBlock* block) { __seqBlockFree(block); }
int pulseq_getBlock(const pulseq_SeqFile* seq, int blockIndex, int parseExtensions, pulseq_SeqBlock* block) { 
    return __getBlock(seq, blockIndex, parseExtensions, block); 
}
