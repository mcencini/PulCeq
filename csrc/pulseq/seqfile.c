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
#include "../vendor.h"

int checkLabelCompatibility(SeqFile *seq);
void buildSparseLabelMap(SeqFile *seq);
int getLabelsByAdcIndex(SeqFile *seq, int adcIndex, LabelEvent *labelOut);
int findLabelMapEntry(SparseLabelMap *map, int adcIndex);

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
    INIT_LIBRARY(seq, rotationQuaternionLibrary, rotationLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, rotationMatrixLibrary, rotationLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, labelsetLibrary, labelsetLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, labelincLibrary, labelincLibrarySize, isExtensionsLibraryParsed);
    /* Initialize sparse label map */
    seq->labelMap.entries = NULL;
    seq->labelMap.size = 0;
    seq->labelMap.capacity = 0;
    seq->areLabelsCompatible = 1; /* Default to compatible until proven otherwise */
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

int __seqFile(char* filePath, SeqFile* seq){
    /* Check for null pointer */
    if (!seq) return 0;
    
    /* Initialize the sequence file */
    seqFileInit(seq);
    
    /* Allocate and copy the file path */
    seq->filePath = (char*) ALLOC(strlen(filePath) + 1);
    strcpy(seq->filePath, filePath);
    
    return 1;
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
        
        /* Free both rotation libraries to be safe */
        FREE(seq->rotationQuaternionLibrary);
        FREE(seq->rotationMatrixLibrary);
        
        FREE(seq->labelsetLibrary);
        FREE(seq->labelincLibrary);
        FREE(seq->labelMap.entries);
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
 * @brief Checks if the labels used in the sequence are compatible with the current vendor configuration.
 *
 * This function examines labelset and labelinc libraries to determine which labels are used,
 * and checks against vendor-defined macros to see if all used labels are supported.
 * 
 * @param seq Pointer to the SeqFile structure.
 * @return 1 if all used labels are compatible, 0 otherwise
 */
int checkLabelCompatibility(SeqFile *seq)
{
    int i;
    int isCompatible = 1;
    int usedLabelTypes[11]; /* Array to track which label types are used (index 0 unused) */
    
    /* Initialize all entries to 0 in C89-compatible way */
    for (i = 0; i < 11; i++) {
        usedLabelTypes[i] = 0;
    }

    /* Check if sequence is valid */
    if (!seq) {
        return 0;
    }

    /* Check if extensions library is parsed */
    if (!seq->isExtensionsLibraryParsed) {
        /* No extensions, so labels are compatible by default */
        return 1;
    }

    /* Check labelset library */
    for (i = 0; i < seq->labelsetLibrarySize; i++) {
        int labelID = (int)seq->labelsetLibrary[i][1];
        if (labelID > 0 && labelID <= 10) { /* Only process counters (1-10), not flags (11+) */
            usedLabelTypes[labelID] = 1;
        }
    }

    /* Check labelinc library */
    for (i = 0; i < seq->labelincLibrarySize; i++) {
        int labelID = (int)seq->labelincLibrary[i][1];
        if (labelID > 0 && labelID <= 10) { /* Only process counters (1-10), not flags (11+) */
            usedLabelTypes[labelID] = 1;
        }
    }

    /* Check compatibility of each used label type against vendor configuration */
    if (usedLabelTypes[SLC] && !LABEL_SUPPORT_SLC) isCompatible = 0;
    if (usedLabelTypes[SEG] && !LABEL_SUPPORT_SEG) isCompatible = 0;
    if (usedLabelTypes[REP] && !LABEL_SUPPORT_REP) isCompatible = 0;
    if (usedLabelTypes[AVG] && !LABEL_SUPPORT_AVG) isCompatible = 0;
    if (usedLabelTypes[SET] && !LABEL_SUPPORT_SET) isCompatible = 0;
    if (usedLabelTypes[ECO] && !LABEL_SUPPORT_ECO) isCompatible = 0;
    if (usedLabelTypes[PHS] && !LABEL_SUPPORT_PHS) isCompatible = 0;
    if (usedLabelTypes[LIN] && !LABEL_SUPPORT_LIN) isCompatible = 0;
    if (usedLabelTypes[PAR] && !LABEL_SUPPORT_PAR) isCompatible = 0;
    if (usedLabelTypes[ACQ] && !LABEL_SUPPORT_ACQ) isCompatible = 0;

    return isCompatible;
}



/**
 * @brief Builds a sparse map of labels for ADC events.
 * 
 * This function computes the actual label values that apply to each ADC event based on
 * the sequence of LABELSET and LABELINC operations in the sequence, storing them in a
 * memory-efficient sparse map. It follows the Pulseq specification rule that label values
 * are recorded only at ADC events.
 *
 * @param seq Pointer to the SeqFile structure.
 */
void buildSparseLabelMap(SeqFile *seq)
{
    int i;
    int blockIndex;
    int adcIndex;
    int extType;
    int extIdx;
    int labelID;
    int labelValue;
    int adcCount;
    LabelEvent currentLabels;
    RawBlock rawBlock;

    /* Check if sequence is valid */
    if (!seq) {
        return;
    }

    /* Initialize labelMap */
    if (!seq->isBlockLibraryParsed || !seq->isAdcLibraryParsed || !seq->isExtensionsLibraryParsed) {
        /* If necessary libraries aren't parsed, just initialize with zero entries */
        seq->labelMap.size = 0;
        seq->labelMap.capacity = 0;
        seq->labelMap.entries = NULL;
        return;
    }

    /* First pass: count the number of ADC events */
    adcCount = 0;
    for (blockIndex = 0; blockIndex < seq->numBlocks; blockIndex++) {
        getRawBlockContentIDs(&rawBlock, seq, blockIndex, 0);
        if (rawBlock.adc >= 1) {
            adcCount++;
        }
    }
    
    /* If no ADC events, return empty map */
    if (adcCount == 0) {
        seq->labelMap.size = 0;
        seq->labelMap.capacity = 0;
        seq->labelMap.entries = NULL;
        return;
    }

    /* Allocate the exact space needed for all ADC events */
    seq->labelMap.size = 0;
    seq->labelMap.capacity = adcCount;
    seq->labelMap.entries = (LabelMapEntry*)ALLOC(sizeof(LabelMapEntry) * adcCount);
    if (!seq->labelMap.entries) {
        seq->labelMap.capacity = 0;
        return;
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

    /* Second pass: calculate label values for each ADC event */
    adcIndex = 0;
    for (blockIndex = 0; blockIndex < seq->numBlocks; blockIndex++) {
        getRawBlockContentIDs(&rawBlock, seq, blockIndex, 1);
        
        /* Process label extensions first, but only if we have extensions */
        for (i = 0; i < rawBlock.extCount; i++) {
            /* Make sure the extension type is valid before accessing */
            extType = seq->extensionLUT[rawBlock.ext[i][0]];
            extIdx = rawBlock.ext[i][1];

            /* Process LABELSET extensions */
            if (extType == EXT_LABELSET) {
                labelID = (int)seq->labelsetLibrary[extIdx][1];
                labelValue = (int)seq->labelsetLibrary[extIdx][0];
                
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
                labelID = (int)seq->labelincLibrary[extIdx][1];
                labelValue = (int)seq->labelincLibrary[extIdx][0];
                
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
        
        /* If this block has an ADC event, store the current label values in the sparse map */
        if (rawBlock.adc >= 1 && seq->labelMap.size < seq->labelMap.capacity) {
            /* Add the new entry */
            seq->labelMap.entries[seq->labelMap.size].adcIndex = adcIndex;
            seq->labelMap.entries[seq->labelMap.size].labels = currentLabels;
            seq->labelMap.size++;
            adcIndex++;
        }
    }
}

void __readSeq(SeqFile *seq)
{
    /* Parse all sequence libraries */
    __readLibraries(seq, 1);
    
    /* Check if extensions library was parsed and if there are labels */
    if (seq->isExtensionsLibraryParsed) {
        /* First check label compatibility with vendor configuration */
        seq->areLabelsCompatible = checkLabelCompatibility(seq);
        
        /* Build the sparse label map after reading all other libraries, but only if labels are compatible */
        if (seq->areLabelsCompatible) {
            /* Build the new sparse label map */
            buildSparseLabelMap(seq);
        }
    } else {
        /* No extensions means no labels, so they're compatible by default */
        seq->areLabelsCompatible = 1;
    }
}

/**
 * @brief Find the index of a label map entry by ADC index
 * 
 * @param map The sparse label map to search
 * @param adcIndex The ADC index to find
 * @return int The index in the map entries array, or -1 if not found
 */
int findLabelMapEntry(SparseLabelMap *map, int adcIndex)
{
    int i;
    
    if (!map || !map->entries || map->size <= 0) {
        return -1;
    }
    
    /* Binary search would be faster for large maps, but linear search is simpler for now */
    for (i = 0; i < map->size; i++) {
        if (map->entries[i].adcIndex == adcIndex) {
            return i;
        }
    }
    
    return -1; /* Not found */
}

/**
 * @brief Get the label values for a specific ADC event
 * 
 * @param seq The sequence file
 * @param adcIndex The index of the ADC event
 * @param labelOut Pointer to a LabelEvent struct to fill with label values
 * @return int 1 if successful, 0 if labels couldn't be retrieved
 */
int getLabelsByAdcIndex(SeqFile *seq, int adcIndex, LabelEvent *labelOut)
{
    int entryIndex;
    
    if (!seq || !labelOut || !seq->areLabelsCompatible) {
        return 0;
    }
    
    /* Initialize all fields to 0 */
    labelOut->slc = 0;
    labelOut->seg = 0;
    labelOut->rep = 0;
    labelOut->avg = 0;
    labelOut->set = 0;
    labelOut->eco = 0;
    labelOut->phs = 0;
    labelOut->lin = 0;
    labelOut->par = 0;
    labelOut->acq = 0;
    
    /* Find the entry in the sparse map */
    entryIndex = findLabelMapEntry(&seq->labelMap, adcIndex);
    
    if (entryIndex < 0) {
        /* Not found in sparse map */
        return 0;
    }
    
    /* Copy the label values */
    *labelOut = seq->labelMap.entries[entryIndex].labels;
    return 1;
}
