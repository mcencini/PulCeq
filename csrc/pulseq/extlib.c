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

    seq->isExtensionsLibraryParsed = 1;
}

void readExtensions(SeqFile* seq)
{
    if (seq->isExtensionsLibraryParsed) return;

    FILE* f = fopen(seq->filePath, "r");
    if (!f) return 1;

    char line[MAX_LINE_LENGTH];
    int maxID = -1;

    /* First pass: find max extension ID to allocate extensionMap */
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (is_space((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '#') continue;

        char name[32];
        int id;
        if (sscanf(p, "extension %31s %d", name, &id) == 2) {
            if (id > maxID) maxID = id;
        }
    }

    if (maxID < 0) {
        fclose(f);
        return 0;  /* No extensions found, still valid */
    }

    seq->extensionMapSize = maxID + 1;
    seq->extensionMap = (int*) ALLOC(sizeof(int) * seq->extensionMapSize);
    if (!seq->extensionMap) {
        fclose(f);
        return 2;
    }

    for (int i = 0; i < seq->extensionMapSize; i++) {
        seq->extensionMap[i] = 0;
    }

    /* Second pass: parse and dispatch */
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (is_space((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '#') continue;

        char name[32];
        int id;
        if (sscanf(p, "extension %31s %d", name, &id) != 2) continue;

        if (strcmp(name, "TRIGGERS") == 0) {
            seq->extensionMap[id] = EXT_TRIGGER;
            readExtensionTriggers(seq, id);
        } else if (strcmp(name, "ROTATIONS") == 0) {
            seq->extensionMap[id] = EXT_ROTATION;
            readExtensionRotations(seq, id);
        } else if (strcmp(name, "RF_SHIM") == 0) {
            seq->extensionMap[id] = EXT_RF_SHIM;
            readExtensionRfShim(seq, id);
        } else if (strcmp(name, "LABELSET") == 0) {
            seq->extensionMap[id] = EXT_LABELSET;
            readExtensionLabelSet(seq, id);
        } else if (strcmp(name, "LABELINC") == 0) {
            seq->extensionMap[id] = EXT_LABELINC;
            readExtensionLabelInc(seq, id);
        } else if (strcmp(name, "DELAYS") == 0) {
            seq->extensionMap[id] = EXT_DELAY;
            readExtensionDelays(seq, id);
        } /* Unknown extensions silently ignored */
    }

    fclose(f);
    seq->isExtensionsLibraryParsed = 1;
    return;
}
