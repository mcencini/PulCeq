/*
 * @file pulseq.c
 * @brief Implementation of public Pulseq API.
 */
#include "pulseq.h"
#include "pulseq/block.h" 
#include "pulseq/seqfile.h" 

SeqFile* seqFile(char* filePath) { return __seqFile(filePath); }
void seqFileFree(SeqFile* seq) { __seqFileFree(seq); }
void seqFileReset(SeqFile* seq) { __seqFileReset(seq); }
void readDefinitions(SeqFile* seq) { __readDefinitions(seq); }
void readLibraries(SeqFile* seq, int readBlocks) { __readLibraries(seq, readBlocks); }
void readSeq(SeqFile* seq) { __readSeq(seq); }

SeqBlock* seqBlock(void) { return __seqBlock(); }
void seqBlockFree(SeqBlock* block) { __seqBlockFree(block); }
SeqBlock* getBlock(const SeqFile* seq, int blockIndex, int parseExtensions) { return __getBlock(seq, blockIndex, parseExtensions); }
