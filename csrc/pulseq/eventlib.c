/**
 * @file eventlib.c
 * @brief Implementation of EventLibraries parsing from Pulseq seqfile.
 *
 */

#include "eventlib.h"

#include "../seqfile.h"

void readDefinitionsLibrary(SeqFile* seq, FILE* f)
{
    int ret;

    /* Check if library was already parsed */
    if (seq->isDefinitionsLibraryParsed) return;

    /* Go to the correct section */
    getSectionOffsets(&(seq->offsets).definitions, seq, f, (const char*[]){"[DEFINITIONS]"}, 1, 0);
    if (seq->offsets.definitions < 0) {
        return;
    }

    /* Preallocate definitions array */
    ret = initDefinitionsLibrary(f, &(seq->offsets).definitions, &seq->definitionsLibrary, &seq->numDefinitions);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize definitionsLibrary\n");
        return;
    }

    /* Second pass — parse values */
    if (fseek(f, seq->offsets.definitions, SEEK_SET) != 0) return;

    char line[MAX_LINE_LENGTH];

    /* Skip section header line */
    if (!fgets(line, sizeof(line), f)) return;

    int defIndex = 0;

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (isspace((unsigned char)*p)) p++;

        if (*p == '\0' || *p == '#') continue;
        if (*p == '[') break;  /* Next section begins */

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
            char** newArray = (char**) ALLOC(sizeof(char*) * (def.valueSize + 1));
            for (int i = 0; i < def.valueSize; i++) {
                newArray[i] = def.value[i];
            }

            newArray[def.valueSize] = (char*) ALLOC(strlen(token) + 1);
            strcpy(newArray[def.valueSize], token);
            if (def.value) FREE(def.value);
            def.value = newArray;
            def.valueSize++;
        }

        /* Assign parsed definition */
        seq->definitionsLibrary[defIndex++] = def;
    }

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
    getSectionOffsets(&(seq->offsets).rf, seq, f, (const char*[]){"[RF]"}, 1, 0);
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
    getSectionOffsets(&(seq->offsets).adc, seq, f, (const char*[]){"[ADC]"}, 1, 0);
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

void readShapesLibrary(SeqFile* seq, FILE* f)
{
    int ret;
    char line[MAX_LINE_LENGTH];
    int shapeIndex;
    int sampleIndex;
    long pos;

    /* Check if library was already parsed */
    if (seq->isShapesLibraryParsed) return;

    /* Go to the correct section */
    getSectionOffsets(&(seq->offsets).shapes, seq, f, (const char*[]){"[SHAPES]"}, 1, 0);
    if (seq->offsets.shapes < 0) {
        return;
    }

    /* Preallocate shapes array */
    ret = initShapesLibrary(f, &(seq->offsets).shapes, &seq->shapesLibrary, &seq->shapesLibrarySize);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize shapesLibrary\n");
        return;
    }

    /* Second pass: Parse and fill waveform data */
    pos = seq->offsets.shapes;
    if (fseek(f, pos, SEEK_SET) != 0) return;

    shapeIndex = -1;
    sampleIndex = 0;

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;

        if (*p == '\0' || *p == '#') continue;
        if (*p == '[') break;

        if (strncmp(p, "shape_id", 8) == 0) {
            shapeIndex++;
            sampleIndex = 0;
            continue;
        }

        if (strncmp(p, "num_samples", 11) == 0) {
            continue;
        }

        if (shapeIndex >= 0 && shapeIndex < seq->shapesLibrarySize) {
            float val;
            if (sscanf(p, "%f", &val) == 1 &&
                sampleIndex < seq->shapesLibrary[shapeIndex].numSamples) {
                seq->shapesLibrary[shapeIndex].samples[sampleIndex++] = val;
            }
        }
    }

    seq->isShapesLibraryParsed = 1;
}
