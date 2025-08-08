/**
 * @file pulseq.c
 * @brief Implementation of public Pulseq API.
 */

#include "pulseq.h"
#include "vendor.h"
#include "pulseq/rotation.c"
#include "pulseq/unique_sequence.h"
#include "pulseq/label.h"
#include "pulseq/seqfile.h"

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

int pulseq_getUniqueSeq(pulseq_SeqFile* uniqueSeq, const pulseq_SeqFile* seq) {
    return getUniqueSeq(uniqueSeq, (const SeqFile*)seq);
}

int pulseq_getNumReadouts(const pulseq_SeqFile* seq, const int excludeNavigator) {
    int numNavigatorADC;
    int i, adcID;
    float* blockRow;
    int extID;
    float* extData;
    int extType;
    int labelID;
    float* labelData;

    /* If excludeNavigator is 0, simply return adcLibrarySize */
    if (!excludeNavigator) {
        return seq->adcLibrarySize;
    }

    /* Check if NAV label is present in the sequence */
    if (!seq->areLabelsCompatible) {
        return seq->adcLibrarySize;
    }

    /* Initialize numNavigatorADC */
    numNavigatorADC = 0;

    /* Loop over blockLibrary */
    for (i = 0; i < seq->numBlocks; i++) {
        blockRow = seq->blockLibrary[i];
        adcID = (int)blockRow[5] - 1; /* ADC ID column */

        if (adcID >= 0) {
            extID = (int)blockRow[6]; /* Extension ID column */

            /* Parse extensions to check for NAV label */
            while (extID > 0 && extID <= seq->extensionsLibrarySize) {
                extData = seq->extensionsLibrary[extID - 1]; /* [type, ref, next_id] */
                extType = (int)extData[0];
                labelID = (int)extData[1] - 1; /* Label ID */
                extID = (int)extData[2]; /* Next extension ID */

                if (extType == EXT_LABELSET && labelID >= 0 && labelID < seq->labelsetLibrarySize) {
                    labelData = seq->labelsetLibrary[labelID];
                    if ((int)labelData[0] != 0) { /* NAV flag */
                        numNavigatorADC++;
                        break; /* Stop checking further extensions for this block */
                    }
                }
            }
        }
    }

    /* Return the number of readouts excluding navigators */
    return seq->adcLibrarySize - numNavigatorADC;
}