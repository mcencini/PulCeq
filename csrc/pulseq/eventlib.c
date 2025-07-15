/**
 * @file eventlib.c
 * @brief Implementation of EventLibraries parsing from Pulseq seqfile.
 *
 */

#include "eventlib.h"

#include "../seqfile.h"

void readDefinitionsLibrary(SeqFile* seq, FILE* f)
{
    /* Check if library was already parsed */
    if (seq->isDefinitionsLibraryParsed) return;

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

    seq->definitionsLibrary = defs;
    seq->numDefinitions = count;

    seq->isDefinitionsLibraryParsed = 1;
}

void readRfLibrary(SeqFile* seq, FILE* f)
{
    int ret;

    Scale rfScale = {
        .size = 10,
        .values = (float[]) {1, 1, 1, 1, 1e-6f, 1e-6f, 1, 1, 1, 1}
    };

    /* Check if library was already parsed */
    if (seq->isRfLibraryParsed) return;

    /* Go to the correct section */
    getSectionOffsets(&(seq->offsets).rf, seq, f, "[RF]", 1, 0);
    if (seq->offsets.rf < 0) {
        return;
    }

    /* Preallocate library */
    ret = initStandardLibrary(f,  &(seq->offsets).rf, 1, &seq->rfLibrary, &seq->rfLibrarySize, rfScale.size);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize rfLibrary\n");
        return;
    }

    /* Parse RF library */
    ret = readStandardLibrary(f, seq->offsets.rf, seq->rfLibrary, seq->rfLibrarySize, rfScale, -1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read rfLibrary from file %s\n", seq->filePath);
        return;
    }

    seq->isRfLibraryParsed = 1;
}

void readGradLibrary(SeqFile* seq, FILE* f)
{
    int ret;

    long offsets[2] = { seq->offsets.grad, seq->offsets.trap };
    int numSections = 0;

    const char* sections[] = { "[GRADIENTS]", "[TRAP]" };

    Scale gradScale = { 
        .size = 6, 
        .values = (float[]){ 1, 1, 1, 1, 1, 1e-6f } 
    };
    Scale trapScale = {
        .size = 5,
        .values = (float[]){ 1, 1e-6f, 1e-6f, 1e-6f, 1e-6f} 
    };

    if (seq->isGradLibraryParsed) return;

    /* Go to the correct section */
    getSectionOffsets(&offsets, seq, f, sections, 2, 0);
    (seq->offsets).grad = offsets[0];
    (seq->offsets).trap = offsets[1];

    /* If SeqFile does not have gradients, exit*/
    if ((seq->offsets).grad >= 0) numSections++;
    if ((seq->offsets).trap >= 0) numSections++;
    if (numSections == 0) {
        return;
    }

    /* Preallocate library */
    ret = initStandardLibrary(f, offsets, 2, &seq->gradLibrary, &seq->gradLibrarySize, gradScale.size + 1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize gradLibrary\n");
        return;
    }

    /* Parse GRADIENTS library */
    if ((seq->offsets).grad >= 0){
        ret = readStandardLibrary(f, offsets[0], seq->gradLibrary, seq->gradLibrarySize, gradScale, 1);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to read gradLibrary ([GRADIENTS] section) from file %s\n", seq->filePath);
            return;
        }
    }

    /* Parse TRAP library */
    if ((seq->offsets).trap >= 0){
        ret = readStandardLibrary(f, offsets[1], seq->gradLibrary, seq->gradLibrarySize, trapScale, 0);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to read gradLibrary ([TRAP] section) from file %s\n", seq->filePath);
            return;
        }
    }

    seq->isGradLibraryParsed = 1;
}

void readAdcLibrary(SeqFile* seq, FILE* f)
{
    int ret;

    Scale adcScale = { 
        .size = 8, 
        .values = (float[]){1, 1e-9f, 1e-6f, 1, 1, 1, 1, 1} 
    };

    /* Check if library was already parsed */
    if (seq->isAdcLibraryParsed) return;

    /* Go to the correct section */
    getSectionOffsets(&(seq->offsets).adc, seq, f, "[ADC]", 1, 0);
    if (seq->offsets.adc < 0) {
        return;
    }

    /* Preallocate library */
    ret = initStandardLibrary(f,  &(seq->offsets).adc, 1, &seq->adcLibrary, &seq->adcLibrarySize, adcScale.size);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize adcLibrary\n");
        return;
    }

    /* Parse ADC library */
    ret = readStandardLibrary(f, seq->offsets.adc, seq->adcLibrary, seq->adcLibrarySize, adcScale, -1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read adcLibrary from file %s\n", seq->filePath);
        return;
    }

    seq->isAdcLibraryParsed = 1;
}