/**
 * @file seqfile.c
 * @brief Implementation of Pulseq seqfile reading.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "pulseq/alloc.h"

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
    int j;
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
        for (i = 0; i < seq->rfShimLibrarySize; i++) {
            FREE(seq->rfShimLibrary[i].values);
        }
        FREE(seq->rfShimLibrary);
    }
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
    FREE(seq->extensionLUT);

    seqFileInit(seq);
}

void readDefinitions(SeqFile* seq)
{
    if (seq->isDefinitionsLibraryParsed) return;
    FILE* f = fopen(seq->filePath, "r");
    if (!f) return;

    char line[MAX_LINE_LENGTH];
    int inSection = 0;
    int count = 0;

    Definition* defs = NULL;

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (isspace((unsigned char)*p)) p++;

        if (*p == '\0' || *p == '#') continue;

        if (!inSection) {
            if (strncmp(p, "[DEFINITIONS]", 13) == 0) {
                inSection = 1;
            }
            continue;
        }

        if (*p == '[') break;  /* Reached next section */

        /* Allocate new definition */
        Definition def;
        def.valueSize = 0;
        def.value = NULL;

        /* Parse name */
        char* nameToken = strtok(p, " \t\r\n");
        if (!nameToken) continue;
        strncpy(def.name, nameToken, DEFINITION_NAME_LENGTH - 1);
        def.name[DEFINITION_NAME_LENGTH - 1] = '\0';

        /* Parse values */
        char* token;
        while ((token = strtok(NULL, " \t\r\n")) != NULL) {
            char** newValueArray = (char**) ALLOC(sizeof(char*) * (def.valueSize + 1));
            for (int i = 0; i < def.valueSize; i++) {
                newValueArray[i] = def.value[i];
            }

            newValueArray[def.valueSize] = (char*) ALLOC(strlen(token) + 1);
            strcpy(newValueArray[def.valueSize], token);
            if (def.value) FREE(def.value);
            def.value = newValueArray;
            def.valueSize++;
        }

        /* Grow global definitions array */
        Definition* newDefs = (Definition*) ALLOC(sizeof(Definition) * (count + 1));
        for (int i = 0; i < count; i++) {
            newDefs[i] = defs[i];
        }
        newDefs[count] = def;
        if (defs) FREE(defs);
        defs = newDefs;
        count++;
    }

    fclose(f);
    seq->definitionsLibrary = defs;
    seq->numDefinitions = count;
    seq->isDefinitionsLibraryParsed = 1;
}

/*************************  Local utils  ****************************************/
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
    seq->offsets.scan_cursor = -1;
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
    INIT_LIBRARY(seq, shapeLibrary, shapeLibrarySize, isShapeLibraryParsed);
}