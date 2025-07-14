/**
 * @file scanfile.c
 * @brief Implementation of Scanfile scanning.
 *
 */


#include "scanfile.h"

#include "../seqfile.h"

/* Local struct to map section tag to offset field */
typedef struct {
    const char* tag;
    long*       field;
} SectionTagMap;

void getSectionOffsets(long* sectionOffsets, SeqFile* seq, FILE* f, const char** sectionNames, int numSections, int parseExtensions)
{
    if (!f || !(seq->offsets).scan_cursor || !seq) return;

    char line[MAX_LINE_LENGTH];
    int foundSections = 0;
    int foundExtensions = 0;
    int totalExtensions = EXT_UNKNOWN;

    /* Track which section names have already been found */
    int sectionFound[numSections];
    for (int i = 0; i < numSections; i++) {
        sectionOffsets[i] = -1;
        sectionFound[i] = 0;
    }

    /* Track which extensions have already been found */
    int extensionFound[EXT_UNKNOWN] = {0};

    /* Begin scan from cursor */
    if (fseek(f, (seq->offsets).scan_cursor, SEEK_SET) != 0) return;

    while (fgets(line, sizeof(line), f)) {
        long pos = ftell(f);
        if (pos < 0) break;

        char* p = line;
        while (*p == ' ' || *p == '\t') p++;

        /* Check for SECTION line */
        if (*p == '[') {
            for (int i = 0; i < numSections; i++) {
                if (!sectionFound[i] && strncmp(p, sectionNames[i], strlen(sectionNames[i])) == 0) {
                    sectionOffsets[i] = pos - strlen(line);  /* store offset of section line */
                    sectionFound[i] = 1;
                    foundSections++;
                }
            }
        }

        /* Check for extension line */
        else if (strncmp(p, "extension", 9) == 0 && (*(p + 9) == ' ' || *(p + 9) == '\t')) {
            char extName[32];
            int extId = -1;
            if (sscanf(p, "extension %31s %d", extName, &extId) == 2) {
                int extEnum = EXT_UNKNOWN;

                if (strcmp(extName, "TRIGGER")  == 0) extEnum = EXT_TRIGGER;
                else if (strcmp(extName, "ROTATION") == 0) extEnum = EXT_ROTATION;
                else if (strcmp(extName, "LABELSET") == 0) extEnum = EXT_LABELSET;
                else if (strcmp(extName, "LABELINC") == 0) extEnum = EXT_LABELINC;
                else if (strcmp(extName, "RF_SHIM")  == 0) extEnum = EXT_RF_SHIM;
                else if (strcmp(extName, "DELAY")    == 0) extEnum = EXT_DELAY;

                seq->extensionMap[extEnum] = extId;
                extensionFound[extEnum] = 1;
                foundExtensions++;
            }
        }

        /* Early exit if all requested section names and all extensions (if required) are found */
        if (foundSections == numSections &&
            (!parseExtensions || foundExtensions == totalExtensions)) {
            (seq->offsets).scan_cursor = ftell(f);
            return;
        }
    }

    /* Full file scanned */
    (seq->offsets).scan_cursor = ftell(f);
}
