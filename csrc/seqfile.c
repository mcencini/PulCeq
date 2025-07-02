/**
 * @file seqfile.c
 * @brief Implementation of Pulseq seqfile reading.
 *
 */
#include "pulSeg.h"
#include "seqfile.h"

SeqFile* seqFile(char* filePath){
    SeqFile *seq = (SeqFile*) ALLOC(sizeof(SeqFile));
    seqFileInit(seq);
    seq->filePath = (char*) ALLOC(strlen(filePath) + 1);
    strcpy(seq->filePath, filePath);
    return seq;
}

void seqFileFree(SeqFile *seq){
    seqFileReset(seq);
    FREE(seq->filePath);
    FREE(seq);
}

void seqFileReset(SeqFile* seq) {
    if (!seq) return;

    int i;
    if (seq->isDefinitionsLibraryParsed && seq->definitionsLibrary) {
        for (i = 0; i < seq->numDefinitions; i++) {
            FREE(seq->definitionsLibrary[i].value);
        }
        FREE(seq->definitionsLibrary);
    }

    if (seq->isAdcLibraryParsed)        FREE(seq->adcLibrary);
    if (seq->isBlockLibraryParsed)      FREE(seq->blockLibrary);
    if (seq->isExtensionsLibraryParsed) FREE(seq->extensionsLibrary);
    if (seq->isGradLibraryParsed)       FREE(seq->gradLibrary);
    if (seq->isLabelincLibraryParsed)   FREE(seq->labelincLibrary);
    if (seq->isLabelsetLibraryParsed)   FREE(seq->labelsetLibrary);
    if (seq->isRfLibraryParsed)         FREE(seq->rfLibrary);

    if (seq->isRfShimLibraryParsed && seq->rfShimLibrary) {
        for (i = 0; i < seq->rfShimLibrarySize; i++) {
            FREE(seq->rfShimLibrary[i]);
        }
        FREE(seq->rfShimLibrary);
        seq->rfShimLibrarySize = 0;
        seq->isRfShimLibraryParsed = 0;
    }

    if (seq->isRotationLibraryParsed) FREE(seq->rotationLibrary);

    if (seq->isShapeLibraryParsed && seq->shapeLibrary) {
        for (i = 0; i < seq->shapeLibrarySize; i++) {
            if (seq->shapeLibrary[i]) {
                FREE(seq->shapeLibrary[i]->samples);
                seq->shapeLibrary[i]->numUncompressedSamples = 0;
                seq->shapeLibrary[i]->numSamples = 0;
                FREE(seq->shapeLibrary[i]);
            }
        }
        FREE(seq->shapeLibrary);
    }

    if (seq->isSoftDelayLibraryParsed) {
        FREE(seq->softDelayLibrary);
        FREE(seq->softDelayHintLibrary);
    }

    if (seq->isTriggerLibraryParsed) FREE(seq->triggerLibrary);

    seqFileInit(seq);
}

/*************************/
/*** Private functions ***/
/*************************/
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
    INIT_LIBRARY(seq, definitionsLibrary, numDefinitions, isDefinitionsLibraryParsed);
    INIT_LIBRARY(seq, adcLibrary, adcLibrarySize, isAdcLibraryParsed);
    INIT_LIBRARY(seq, blockLibrary, blockLibrarySize, isBlockLibraryParsed);
    INIT_LIBRARY(seq, extensionsLibrary, extensionsLibrarySize, isExtensionsLibraryParsed);
    INIT_LIBRARY(seq, gradLibrary, gradLibrarySize, isGradLibraryParsed);
    INIT_LIBRARY(seq, labelincLibrary, labelincLibrarySize, isLabelincLibraryParsed);
    INIT_LIBRARY(seq, labelsetLibrary, labelsetLibrarySize, isLabelsetLibraryParsed);
    INIT_LIBRARY(seq, rfLibrary, rfLibrarySize, isRfLibraryParsed);
    INIT_LIBRARY(seq, rfShimLibrary, rfShimLibrarySize, isRfShimLibraryParsed);
    INIT_LIBRARY(seq, rotationLibrary, rotationLibrarySize, isRotationLibraryParsed);
    INIT_LIBRARY(seq, shapeLibrary, shapeLibrarySize, isShapeLibraryParsed);
    INIT_LIBRARY(seq, softDelayLibrary, softDelayLibrarySize, isSoftDelayLibraryParsed);
    INIT_LIBRARY(seq, triggerLibrary, triggerLibrarySize, isTriggerLibraryParsed);
    seq->softDelayHintLibrary = NULL;
}


/**
 * @brief Map string label names to integer label codes.
 * 
 * @param[in] label The string representation of the label (e.g., "LIN").
 * @return int The corresponding numeric code, or -1 if not recognized.
 */
int parseLabelType(const char *label) {
    if (label == NULL) return -1;

    struct {
        const char *name;
        int value;
    } static const labelTable[] = {
        { "SLC", SLC },
        { "SEG", SEG },
        { "REP", REP },
        { "AVG", AVG },
        { "SET", SET },
        { "ECO", ECO },
        { "PHS", PHS },
        { "LIN", LIN },
        { "PAR", PAR },
        { "ACQ", ACQ },
        { "TRID", TRID },
        { "NAV", NAV },
        { "REV", REV },
        { "SMS", SMS },
        { "REF", REF },
        { "IMA", IMA },
        { "NOISE", NOISE },
        { "PMC", PMC },
        { "NOROT", NOROT },
        { "NOPOS", NOPOS },
        { "NOSCL", NOSCL },
        { "ONCE", ONCE },
        { NULL, -1 }  /* Sentinel */
    };

    for (int i = 0; labelTable[i].name != NULL; i++) {
        if (strcmp(label, labelTable[i].name) == 0) {
            return labelTable[i].value;
        }
    }

    return -1;  /* Not found */
}