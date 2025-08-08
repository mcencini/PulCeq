/**
 * @file pulseq.c
 * @brief Implementation of public Pulseq API.
 */

#include "pulseq.h"
#include "vendor.h"
#include "pulseq/rotation.c"

int pulseq_seqFile(char* filePath, pulseq_SeqFile* seq) { 
    int result;
    int i;
    
    result = __seqFile(filePath, seq); 
    
    /* If using matrix representation, convert quaternions to matrices */
    if (result && ROTATION_FORMAT == ROTATION_FORMAT_MATRIX && seq->rotationQuaternionLibrary) {
        /* Allocate rotation matrix library if quaternion library exists */
        if (seq->rotationLibrarySize > 0) {
            seq->rotationMatrixLibrary = (float(*)[9])ALLOC(seq->rotationLibrarySize * sizeof(float[9]));
            if (!seq->rotationMatrixLibrary) {
                /* Handle allocation failure */
                return 0;
            }
            
            /* Convert each quaternion to a rotation matrix */
            for (i = 0; i < seq->rotationLibrarySize; i++) {
                quaternionToMatrix(seq->rotationQuaternionLibrary[i], seq->rotationMatrixLibrary[i]);
            }
            
            /* Free quaternion library as it's no longer needed */
            FREE(seq->rotationQuaternionLibrary);
            seq->rotationQuaternionLibrary = NULL;
        }
    }
    
    return result;
}

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

int pulseq_getLabelsForAdc(const pulseq_SeqFile* seq, int adcIndex, pulseq_LabelEvent* labelOut) {
    return getLabelsByAdcIndex((SeqFile*)seq, adcIndex, labelOut);
}
