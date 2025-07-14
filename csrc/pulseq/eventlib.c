/**
 * @file eventlib.c
 * @brief Implementation of EventLibraries parsing from Pulseq seqfile.
 *
 */

#include "eventlib.h"

#include "../seqfile.h"

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