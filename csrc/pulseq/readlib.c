/**
 * @file readlib.c
 * @brief Implementation of common EventLibrary reading subroutines.
 *
 */

#include "readlib.h"

#include "../seqfile.h"

int initStandardLibrary(FILE* f, const long* offsets, int numSections, float*** target, int* targetCount, int numEntries)
{
    if (!f) return 1;

    char line[MAX_LINE_LENGTH];
    int maxIndex = -1;

    for (int sec = 0; sec < numSections; sec++) {
        if (offsets[sec] < 0) continue;  /* Skip not found */

        if (fseek(f, offsets[sec], SEEK_SET) != 0) {
            return 1;
        }

        /* Skip the section header line */
        if (!fgets(line, sizeof(line), f)) {
            return 1;
        }

        /* Read until next section or EOF */
        while (fgets(line, sizeof(line), f)) {
            char* p = line;
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '[') break; /* Next section starts */

            if (*p == '\0' || *p == '#') continue; /* Skip blank/comment */

            int idx = -1;
            if (sscanf(p, "%d", &idx) == 1) {
                if (idx > maxIndex) maxIndex = idx;
            }
        }
    }

    if (maxIndex < 0) {
        *target = NULL;
        *targetCount = 0;
        return 1; /* No entries found */
    }

    /* Allocate zero-filled 2D array */
    float** array = (float**) ALLOC(sizeof(float*) * (maxIndex + 1));
    if (!array) return 5;

    for (int i = 0; i <= maxIndex; i++) {
        array[i] = (float*) ALLOC(sizeof(float) * numEntries);
        if (!array[i]) {
            for (int j = 0; j < i; j++) FREE(array[j]);
            FREE(array);
            return 6;
        }
        for (int j = 0; j < numEntries; j++) {
            array[i][j] = 0.0f;
        }
    }

    *target = array;
    *targetCount = maxIndex + 1;
    return 0;
}

int initRfShimLibrary(FILE* f, long offset, RfShimEntry** target, int* targetCount)
{
    if (!f || !target || !targetCount) return 1;

    char line[MAX_LINE_LENGTH];
    int maxIndex = -1;

    if (fseek(f, offset, SEEK_SET) != 0) return 1;

    /* Skip the section header line */
    if (!fgets(line, sizeof(line), f)) return 1;

    /* First pass: determine max index */
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;

        int idx;
        if (sscanf(p, "%d", &idx) == 1 && idx > maxIndex) {
            maxIndex = idx;
        }
    }

    if (maxIndex < 0) return 1;

    /* Allocate array of RfShimEntry */
    RfShimEntry* array = (RfShimEntry*) ALLOC(sizeof(RfShimEntry) * (maxIndex + 1));
    if (!array) return 1;

    for (int i = 0; i <= maxIndex; i++) {
        array[i].nChannels = 0;
        array[i].values = NULL;
    }

    *target = array;
    *targetCount = maxIndex + 1;
    return 0;
}

int readStandardLibrary(FILE* f, long offset, float** target, int targetCount, Scale scale, int flag)
{
    char line[MAX_LINE_LENGTH];

    if (!f) return 1;

    if (fseek(f, offset, SEEK_SET) != 0) {
        return 1;
    }

    /* Skip section header line */
    if (!fgets(line, sizeof(line), f)) {
        return 1;
    }

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[') break; /* next section */

        if (*p == '\0' || *p == '#') continue;

        int idx;
        if (sscanf(p, "%d", &idx) != 1) continue;
        if (idx < 0 || idx >= targetCount) continue;

        /* Move pointer past index */
        while (*p && *p != ' ' && *p != '\t') p++;
        while (*p == ' ' || *p == '\t') p++;

        float vals[scale.size];
        int parsed = 0;
        char* scanPtr = p;

        for (int n = 0; n < scale.size; n++) {
            float v;
            int consumed = 0;
            if (sscanf(scanPtr, "%f%n", &v, &consumed) != 1) break;
            vals[n] = v;
            scanPtr += consumed;
            while (*scanPtr == ' ' || *scanPtr == '\t') scanPtr++;
            parsed++;
        }

        if (parsed != scale.size) continue;

        int offsetCol = (flag >= 0) ? 1 : 0;
        for (int n = 0; n < scale.size; n++) {
            target[idx][n + offsetCol] = vals[n] * scale.values[n];
        }
        if (flag >= 0) {
            target[idx][0] = (float)flag;
        }
    }

    return 0;
}

int readLabelLibrary(FILE* f, long offset, float** target, int targetCount, int* isLabelDefined) {
    if (!f || offset < 0) return 1;

    char line[MAX_LINE_LENGTH];
    if (fseek(f, offset, SEEK_SET) != 0) return 1;

    /* Skip section header line */
    fgets(line, sizeof(line), f);

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;

        int idx;
        float val;
        char label[LABEL_NAME_LENGTH];

        if (sscanf(p, "%d %f %31s", &idx, &val, label) == 3 &&
            idx >= 0 && idx < targetCount) {

            int labelCode = label2enum(label);
            
            /* bookkeep found labels and flags */
            if (labelCode > 0){
                isLabelDefined[labelCode] = 1;
            }

            target[idx][0] = val;
            target[idx][1] = (float)labelCode;
        }
    }

    return 0;
}

int readDelayLibrary(FILE* f, long offset, float** target, int targetCount) {
    if (!f || offset < 0) return 1;

    char line[MAX_LINE_LENGTH];
    if (fseek(f, offset, SEEK_SET) != 0) return 1;
    
    /* Skip section header line */
    fgets(line, sizeof(line), f);

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;

        int idx;
        float offsetVal, scaleVal;
        char hint[SOFT_DELAY_HINT_LENGTH];

        if (sscanf(p, "%d %f %f %31s", &idx, &offsetVal, &scaleVal, hint) == 4 &&
            idx >= 0 && idx < targetCount) {

            int hintCode = hint2enum(hint);

            target[idx][0] = offsetVal;
            target[idx][1] = scaleVal;
            target[idx][2] = (float)hintCode;
        }
    }

    return 0;
}

int readRfShimLibrary(FILE* f, long offset, RfShimEntry* target, int targetCount)
{
    if (!f || !target) return 1;

    char line[MAX_LINE_LENGTH];
    if (fseek(f, offset, SEEK_SET) != 0) return 1;

    /* Skip section header line */
    if (!fgets(line, sizeof(line), f)) return 1;

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;

        int idx, nCh;
        if (sscanf(p, "%d %d", &idx, &nCh) != 2) continue;
        if (idx < 0 || idx >= targetCount || nCh <= 0) continue;

        /* Skip past the index and nCh */
        while (*p && *p != ' ') p++; while (*p == ' ') p++;
        while (*p && *p != ' ') p++; while (*p == ' ') p++;

        float* values = (float*) ALLOC(sizeof(float) * 2 * nCh);
        if (!values) return 1;

        for (int i = 0; i < 2 * nCh; i++) {
            float val;
            int consumed = 0;
            if (sscanf(p, "%f%n", &val, &consumed) != 1) {
                FREE(values);
                break;
            }
            values[i] = val;
            p += consumed;
            while (*p == ' ' || *p == '\t') p++;
        }

        target[idx].nChannels = nCh;
        target[idx].values = values;
    }

    return 0;
}

/**************************************************** local utils /****************************************************/
int label2enum(const char *label) {
    if (!label) return -1;

    struct {
        const char *name;
        int value;
    } static const table[] = {
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
        { NULL, -1 }
    };

    for (int i = 0; table[i].name != NULL; i++) {
        if (strcmp(label, table[i].name) == 0) return table[i].value;
    }

    return -1;
}

int hint2enum(const char *hint) {
    if (!hint) return -1;

    struct {
        const char *name;
        int value;
    } static const table[] = {
        { "TE", HINT_TE }, 
        { "TR", HINT_TR },
        { "TI", HINT_TI }, 
        { "ESP", HINT_ESP },
        { "RECTIME", HINT_RECTIME },
        { "T2PREP", HINT_T2PREP }, 
        { "TE2", HINT_TE2 },
        { "TR2", HINT_TR2 },
        { NULL, -1 }
    };

    for (int i = 0; table[i].name != NULL; i++) {
        if (strcmp(hint, table[i].name) == 0) return table[i].value;
    }

    return -1;
}

