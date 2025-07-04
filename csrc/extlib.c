/**
 * @file extlib.c
 * @brief Implementation of extensions parsing from Pulseq seqfile.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "constants.h"
#include "c_type.h"
#include "extlib.h"
#include "seqfile.h"

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
