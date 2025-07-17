/**
 * @file scanfile.c
 * @brief Implementation of Scanfile scanning.
 *
 */

#include "seqfile.h"

#include "scanfile.h"

/* Local struct to map section tag to offset field */
typedef struct {
    const char* tag;
    long*       field;
} SectionTagMap;

void getSectionOffsets(long* sectionOffsets, SeqFile* seq, FILE* f, const char** sectionNames, int numSections, int parseExtensions)
{
    char line[MAX_LINE_LENGTH];
    int foundSections = 0;
    int foundExtensions = 0;
    int totalExtensions = EXT_UNKNOWN;
    int i;
    int* sectionFound = NULL;
    int* extensionFound = NULL;
    char extName[EXT_NAME_LENGTH];
    int extId;
    int extEnum;
    char* p;
    long pos;

    /* Validate input */
    if (!f || !(seq->offsets).scan_cursor || !seq) return;

    /* Allocate dynamic arrays */
    sectionFound = (int*) ALLOC(sizeof(int) * numSections);
    if (!sectionFound) return;

    extensionFound = (int*) ALLOC(sizeof(int) * totalExtensions);
    if (!extensionFound) {
        FREE(sectionFound);
        return;
    }

    /* Initialize arrays */
    for (i = 0; i < numSections; i++) {
        sectionOffsets[i] = -1;
        sectionFound[i] = 0;
    }

    for (i = 0; i < totalExtensions; i++) {
        extensionFound[i] = 0;
    }

    /* Begin scan from cursor */
    if (fseek(f, (seq->offsets).scan_cursor, SEEK_SET) != 0) {
        FREE(sectionFound);
        FREE(extensionFound);
        return;
    }

    while (fgets(line, sizeof(line), f)) {
        pos = ftell(f);
        if (pos < 0) break;

        p = line;
        while (*p == ' ' || *p == '\t') p++;

        /* Check for SECTION line */
        if (*p == '[') {
            for (i = 0; i < numSections; i++) {
                if (!sectionFound[i] && strncmp(p, sectionNames[i], strlen(sectionNames[i])) == 0) {
                    sectionOffsets[i] = pos - strlen(line);  /* store offset of section line */
                    sectionFound[i] = 1;
                    foundSections++;
                }
            }
        }

        /* Check for extension line */
        else if (strncmp(p, "extension", 9) == 0 && (*(p + 9) == ' ' || *(p + 9) == '\t')) {
            extId = -1;
            extEnum = EXT_UNKNOWN;
            if (sscanf(p, "extension %31s %d", extName, &extId) == 2) {
                if (strcmp(extName, "TRIGGER") == 0) extEnum = EXT_TRIGGER;
                else if (strcmp(extName, "ROTATION") == 0) extEnum = EXT_ROTATION;
                else if (strcmp(extName, "LABELSET") == 0) extEnum = EXT_LABELSET;
                else if (strcmp(extName, "LABELINC") == 0) extEnum = EXT_LABELINC;
                else if (strcmp(extName, "RF_SHIM") == 0) extEnum = EXT_RF_SHIM;
                else if (strcmp(extName, "DELAY") == 0) extEnum = EXT_DELAY;

                seq->extensionMap[extEnum] = extId;
                extensionFound[extEnum] = 1;
                foundExtensions++;
            }
        }

        /* Early exit if all requested sections and extensions found */
        if (foundSections == numSections && (!parseExtensions || foundExtensions == totalExtensions)) {
            (seq->offsets).scan_cursor = ftell(f);
            break;
        }
    }

    /* Full file scanned or early exit */
    (seq->offsets).scan_cursor = ftell(f);

    FREE(sectionFound);
    FREE(extensionFound);
}