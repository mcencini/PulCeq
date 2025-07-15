/**
 * @file extlib.c
 * @brief Implementation of extensions parsing from Pulseq seqfile.
 *
 */

#include "extlib.h"

#include "../seqfile.h"

void readExtensionLibrary(SeqFile* seq, FILE* f)
{
    int ret;

    Scale extScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };

    Scale triggerScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };

    Scale rotationsScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };

    Scale labelsetScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };

    Scale labelincScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };
    
    Scale delaysScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };

    Scale rfshimScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };

    /* Check if library was already parsed */
    if (seq->isExtensionsLibraryParsed) return;

    /* Go to the correct section */
    getSectionOffsets(&(seq->offsets).extensions, seq, f, "[EXTENSIONS]", 1, 1);
    if (seq->offsets.extensions < 0) {
        return;
    }

    /* Preallocate library */
    ret = initStandardLibrary(f, seq->offsets.extensions, 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize extensionsLibrary\n");
        return;
    }

    if (seq->offsets.triggers > 0){
        ret = initStandardLibrary(f, seq->offsets.triggers, 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize trigger library\n");
            return;
        }
    }

    if (seq->offsets.rotations > 0){
        ret = initStandardLibrary(f, seq->offsets.rotations, 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize rotations library\n");
            return;
        }
    }

    if (seq->offsets.labelset > 0){
        ret = initStandardLibrary(f, seq->offsets.labelset, 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize labelset library\n");
            return;
        }
    }

    if (seq->offsets.labelinc > 0){
        ret = initStandardLibrary(f, seq->offsets.labelinc, 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize labelinc library\n");
            return;
        }
    }

    if (seq->offsets.delays > 0){
        ret = initStandardLibrary(f, seq->offsets.delays, 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize delays library\n");
            return;
        }
    }

    if (seq->offsets.rfshim > 0){
        ret = initStandardLibrary(f, seq->offsets.rfshim, 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize rf shim library\n");
            return;
        }
    }

    /* Parse Extensions library */
    ret = readStandardLibrary(f, seq->offsets.extensions, seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read extensionsLibrary from file %s\n", seq->filePath);
        return;
    }

    if (seq->offsets.triggers > 0){
        ret = readStandardLibrary(f, seq->offsets.triggers, seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize trigger library\n");
            return;
        }
    }

    if (seq->offsets.rotations > 0){
        ret = readStandardLibrary(f, seq->offsets.rotations, seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize rotations library\n");
            return;
        }
    }

    if (seq->offsets.labelset > 0){
        ret = readStandardLibrary(f, seq->offsets.labelset, seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize labelset library\n");
            return;
        }
    }

    if (seq->offsets.labelinc > 0){
        ret = readStandardLibrary(f, seq->offsets.labelinc, seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize labelinc library\n");
            return;
        }
    }

    if (seq->offsets.delays > 0){
        ret = readStandardLibrary(f, seq->offsets.delays, seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize delays library\n");
            return;
        }
    }

    if (seq->offsets.rfshim > 0){
        ret = readStandardLibrary(f, seq->offsets.rfshim, seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
        if (ret != 0) {
            fprintf(stderr, "Error: Failed to initialize rf shim library\n");
            return;
        }
    }

    seq->isExtensionsLibraryParsed = 1;
}