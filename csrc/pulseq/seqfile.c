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
#include "block.h"
#include "constants.h"
#include "eventlib.h"
#include "extlib.h"
#include "scanfile.h"

/*********************************************************  local utils  *********************************************************/
void fillLabelLibrary(SeqFile *seq);

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
    seq->labelLibrary = NULL;
    seq->labelLibrarySize = 0;
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
        FREE(seq->labelLibrary);
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

/**
 * @brief Fills the labelLibrary array with label values for each ADC event.
 *
 * This function computes the actual label values that apply to each ADC event based on
 * the sequence of LABELSET and LABELINC operations in the sequence. It follows the Pulseq
 * specification rule that label values are recorded only at ADC events.
 *
 * @param seq Pointer to the SeqFile structure.
 */
void fillLabelLibrary(SeqFile *seq)
{
    int i, j, blockIndex, adcID, extType, extIdx, labelID, labelValue;
    int nextAdcIndex = 0;
    LabelEvent currentLabels;
    RawBlock rawBlock;

    /* Check if sequence is valid */
    if (!seq) {
        return;
    }

    /* Initialize labelLibrary */
    if (!seq->isBlockLibraryParsed || !seq->isAdcLibraryParsed || !seq->isExtensionsLibraryParsed) {
        /* If necessary libraries aren't parsed, just initialize with zero entries */
        seq->labelLibrarySize = 0;
        seq->labelLibrary = NULL;
        return;
    }

    /* Initialize labelLibrary with same size as adcLibrary */
    seq->labelLibrarySize = seq->adcLibrarySize;
    seq->labelLibrary = (LabelEvent*)ALLOC(sizeof(LabelEvent) * seq->labelLibrarySize);
    if (!seq->labelLibrary) {
        seq->labelLibrarySize = 0;
        return;
    }
    
    /* Initialize all labelLibrary entries to 0 */
    for (i = 0; i < seq->labelLibrarySize; i++) {
        seq->labelLibrary[i].slc = 0;
        seq->labelLibrary[i].seg = 0;
        seq->labelLibrary[i].rep = 0;
        seq->labelLibrary[i].avg = 0;
        seq->labelLibrary[i].set = 0;
        seq->labelLibrary[i].eco = 0;
        seq->labelLibrary[i].phs = 0;
        seq->labelLibrary[i].lin = 0;
        seq->labelLibrary[i].par = 0;
        seq->labelLibrary[i].acq = 0;
    }

    /* Initialize all label counters to 0 at the start of the sequence */
    currentLabels.slc = 0;
    currentLabels.seg = 0;
    currentLabels.rep = 0;
    currentLabels.avg = 0;
    currentLabels.set = 0;
    currentLabels.eco = 0;
    currentLabels.phs = 0;
    currentLabels.lin = 0;
    currentLabels.par = 0;
    currentLabels.acq = 0;

    /* Iterate through all blocks in sequence order */
    for (blockIndex = 0; blockIndex < seq->numBlocks; blockIndex++) {
        if (nextAdcIndex >= seq->labelLibrarySize) {
            break;
        }

        getRawBlockContentIDs(&rawBlock, seq, blockIndex, 1);
        
        /* Process label extensions first, but only if we have extensions */
        for (i = 0; i < rawBlock.extCount; i++) {
            /* Make sure the extension type is valid before accessing */
            extType = seq->extensionLUT[rawBlock.ext[i][0]];
            extIdx = rawBlock.ext[i][1];

            /* Process LABELSET extensions */
            if (extType == EXT_LABELSET) {
                labelID = seq->labelsetLibrary[extIdx][1];
                labelValue = seq->labelsetLibrary[extIdx][0];
                
                /* Update label counters (not flags) */
                switch (labelID) {
                    case SLC: currentLabels.slc = labelValue; break;
                    case SEG: currentLabels.seg = labelValue; break;
                    case REP: currentLabels.rep = labelValue; break;
                    case AVG: currentLabels.avg = labelValue; break;
                    case SET: currentLabels.set = labelValue; break;
                    case ECO: currentLabels.eco = labelValue; break;
                    case PHS: currentLabels.phs = labelValue; break;
                    case LIN: currentLabels.lin = labelValue; break;
                    case PAR: currentLabels.par = labelValue; break;
                    case ACQ: currentLabels.acq = labelValue; break;
                    default: break;  /* Skip flags */
                }
            }

            /* Process LABELINC extensions */
            else if (extType == EXT_LABELINC) {
                labelID = seq->labelincLibrary[extIdx][1];
                labelValue = seq->labelincLibrary[extIdx][0];
                
                /* Update label counters */
                switch (labelID) {
                    case SLC: currentLabels.slc += labelValue; break;
                    case SEG: currentLabels.seg += labelValue; break;
                    case REP: currentLabels.rep += labelValue; break;
                    case AVG: currentLabels.avg += labelValue; break;
                    case SET: currentLabels.set += labelValue; break;
                    case ECO: currentLabels.eco += labelValue; break;
                    case PHS: currentLabels.phs += labelValue; break;
                    case LIN: currentLabels.lin += labelValue; break;
                    case PAR: currentLabels.par += labelValue; break;
                    case ACQ: currentLabels.acq += labelValue; break;
                    default: break;
                }
            }
        }
        
        /* If this block has an ADC event, store the current label values */
        if (rawBlock.adc >= 1 && nextAdcIndex < seq->labelLibrarySize) {
            seq->labelLibrary[nextAdcIndex] = currentLabels;
            nextAdcIndex++;
        }
    }
}

void __readSeq(SeqFile *seq)
{
    __readLibraries(seq, 1);
    
    /* Fill the labelLibrary after reading all other libraries */
    fillLabelLibrary(seq);
}
