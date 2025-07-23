/**
 * @file seqfile.c
 * @brief Implementation of Pulseq seqfile reading.
 *
 */

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "seqfile.h"

#include "alloc.h"
#include "eventlib.h"
#include "extlib.h"
#include "scanfile.h"

/*********************************************************  local utils  *********************************************************/
#define INIT_LIBRARY(seq, fieldPtr, sizeField, flagField) \
    do { \
        (seq)->fieldPtr = NULL; \
        (seq)->sizeField = 0; \
        (seq)->flagField = 0; \
    } while (0)
    
/**
 * @brief Initialize SeqFile fields.
 * 
 * @param[in] seq The uninitialized SeqFile structure.
 */
void seqFileInit(SeqFile* seq){
    int i;
    seq->offsets.scan_cursor = 0;
    seq->offsets.version = -1;
    seq->offsets.definitions = -1;
    seq->offsets.blocks = -1;
    seq->offsets.rf = -1;
    seq->offsets.grad = -1;
    seq->offsets.trap = -1;
    seq->offsets.adc = -1;
    seq->offsets.extensions = -1;
    seq->offsets.triggers = -1;
    seq->offsets.rfshim = -1;
    seq->offsets.labelset = -1;
    seq->offsets.labelinc = -1;
    seq->offsets.delays = -1;
    seq->offsets.rotations = -1;
    seq->offsets.shapes = -1;
    seq->offsets.signature = -1;

    INIT_LIBRARY(seq, definitionsLibrary, numDefinitions, isDefinitionsLibraryParsed);
    INIT_LIBRARY(seq, blockLibrary, numBlocks, isBlockLibraryParsed);
    INIT_LIBRARY(seq, rfLibrary, rfLibrarySize, isRfLibraryParsed);
    INIT_LIBRARY(seq, gradLibrary, gradLibrarySize, isGradLibraryParsed);
    INIT_LIBRARY(seq, adcLibrary, adcLibrarySize, isAdcLibraryParsed);
    INIT_LIBRARY(seq, extensionsLibrary, extensionsLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, triggerLibrary, triggerLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, rotationLibrary, rotationLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, labelsetLibrary, labelsetLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, labelincLibrary, labelincLibrarySize, isExtensionsLibraryParsed);
    for (i = 0; i < 22; i++){
        seq->isLabelDefined[i] = 0;
    }
    INIT_LIBRARY(seq, softDelayLibrary, softDelayLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, rfShimLibrary, rfShimLibrarySize, isExtensionsLibraryParsed);
    for (i = 0; i < 8; i++){
        seq->extensionMap[i] = -1;
    }
    seq->extensionLUTSize = 0;
    seq->extensionLUT = NULL;
    INIT_LIBRARY(seq, shapesLibrary, shapesLibrarySize, isShapesLibraryParsed);
}
/****************************************************  end local utils  ****************************************************/

SeqFile* __seqFile(char* filePath){
    SeqFile *seq = (SeqFile*) ALLOC(sizeof(SeqFile));
    seqFileInit(seq);
    seq->filePath = (char*) ALLOC(strlen(filePath) + 1);
    strcpy(seq->filePath, filePath);
    return seq;
}

void __seqFileFree(SeqFile *seq){
    __seqFileReset(seq);
    FREE(seq->filePath);
    FREE(seq);
}

void __seqFileReset(SeqFile* seq) {
    int i, j;
    if (!seq) return;
    if (seq->isDefinitionsLibraryParsed && seq->definitionsLibrary) {
        for (i = 0; i < seq->numDefinitions; i++) {
            FREE(seq->definitionsLibrary[i].value);
        }
        FREE(seq->definitionsLibrary);
    }
    if (seq->isBlockLibraryParsed)      FREE(seq->blockLibrary);
    if (seq->isRfLibraryParsed)         FREE(seq->rfLibrary);
    if (seq->isGradLibraryParsed)       FREE(seq->gradLibrary);
    if (seq->isAdcLibraryParsed)        FREE(seq->adcLibrary);
    if (seq->isExtensionsLibraryParsed) {
        FREE(seq->extensionsLibrary);
        FREE(seq->triggerLibrary);
        FREE(seq->rotationLibrary);
        FREE(seq->labelsetLibrary);
        FREE(seq->labelincLibrary);
        FREE(seq->softDelayLibrary);
        FREE(seq->rfShimLibrary);
    }
    if (seq->isShapesLibraryParsed && seq->shapesLibrary) {
        for (i = 0; i < seq->shapesLibrarySize; i++) {
            FREE(seq->shapesLibrary[i].samples);
            seq->shapesLibrary[i].samples = NULL;
            seq->shapesLibrary[i].numUncompressedSamples = 0;
            seq->shapesLibrary[i].numSamples = 0;
        }
        FREE(seq->shapesLibrary);
    }

    FREE(seq->extensionLUT);
    
    seqFileInit(seq);
}

void __readDefinitions(SeqFile* seq)
{
    FILE* f = fopen(seq->filePath, "r");
    
    if (!f) return;
    readDefinitionsLibrary(seq, f);    
    fclose(f);
    
    return;
}

void __readLibraries(SeqFile* seq, int readBlocks)
{
    FILE* f = fopen(seq->filePath, "r");
    
    if (!f) return;
    getSectionOffsets(seq, f);
    readVersion(seq, f);
    if (seq->versionCombined < 1005000) {
        fprintf(stderr, "Error: Unsupported sequence file version %d.%d.%d\n", seq->versionMajor, seq->versionMinor, seq->versionRevision);
        fclose(f);
        return;
    }
    readDefinitionsLibrary(seq, f); 
    if (readBlocks) {
        readBlockLibrary(seq, f);
    }
    readRfLibrary(seq, f);
    readGradLibrary(seq, f);
    readAdcLibrary(seq, f);
    readShapesLibrary(seq, f);
    readExtensionsLibrary(seq, f);      
    fclose(f);
    
    return;
}

void __readSeq(SeqFile *seq)
{
    __readLibraries(seq, 1);
}
