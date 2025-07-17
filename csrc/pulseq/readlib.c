/**
 * @file readlib.c
 * @brief Implementation of common EventLibrary reading subroutines.
 *
 */

#include "seqfile.h"

#include "readlib.h"

/****************************************************  local utils  ****************************************************/
typedef struct {
    const char *name;
    int value;
} TableEntry;

static const TableEntry label_table[] = {
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

int label2enum(const char *label) {
    int i;
    if (!label) return -1;
    for (i = 0; label_table[i].name != NULL; i++) {
        if (strcmp(label, label_table[i].name) == 0) return label_table[i].value;
    }
    return -1;
}

static const TableEntry hint_table[] = {
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

int hint2enum(const char *hint) {
    int i;
    if (!hint) return -1;
    for (i = 0; hint_table[i].name != NULL; i++) {
        if (strcmp(hint, hint_table[i].name) == 0) return hint_table[i].value;
    }
    return -1;
}
/****************************************************  end local utils  ****************************************************/

int initStandardLibrary(FILE* f, const long* offsets, int numSections, void** target, int* targetCount, int N)
{
    char line[MAX_LINE_LENGTH];
    int maxIndex = -1;
    int sec, i, j, idx;
    char* p;
    float *array_raw;

    if (!f) return 1;
    for (sec = 0; sec < numSections; sec++) {
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
            p = line;
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '[') break; /* Next section starts */
            if (*p == '\0' || *p == '#') continue; /* Skip blank/comment */
            if (sscanf(p, "%d", &idx) == 1) {
                if (idx > maxIndex) maxIndex = idx;
            }
        }
    }

    if (maxIndex <= 0) {
        *target = NULL;
        *targetCount = 0;
        return 1; /* No entries found */
    }

    /* Allocate zero-filled 2D array as a single block */
    array_raw = (float*) ALLOC(sizeof(float) * N * (maxIndex + 1));
    if (!array_raw) return 1;
    for (i = 0; i < (maxIndex + 1) * N; i++) {
        array_raw[i] = 0.0f;
    }
    *target = (void*)array_raw;
    *targetCount = maxIndex + 1;
    return 0;
}

int initDefinitionsLibrary(FILE* f, long offset, Definition** target, int* targetCount)
{
    char line[MAX_LINE_LENGTH];
    int count = 0;
    int inSection = 0;
    char* p;
    char* nameToken;
    Definition* defs;

    if (!f || offset < 0 || !target || !targetCount) return 1;

    if (fseek(f, offset, SEEK_SET) != 0) return 2;

    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (isspace((unsigned char)*p)) p++;

        if (*p == '\0' || *p == '#') continue;

        if (!inSection) {
            if (strncmp(p, "[DEFINITIONS]", 13) == 0) {
                inSection = 1;
            }
            continue;
        }

        if (*p == '[') break;  /* Reached next section */

        /* Count valid definition lines */
        nameToken = strtok(p, " \t\r\n");
        if (nameToken) count++;
    }

    if (count == 0) {
        *target = NULL;
        *targetCount = 0;
        return 1;  /* no definitions found */
    }

    defs = (Definition*) ALLOC(sizeof(Definition) * count);
    if (!defs) return 1;

    *target = defs;
    *targetCount = count;
    return 0;
}

int initShapesLibrary(FILE* f, long offset, ShapeArbitrary** target, int* targetCount)
{
    char line[MAX_LINE_LENGTH];
    int count, currentIndex, num, n, i;
    ShapeArbitrary* shapes;
    char* p;

    if (!f || !offset || !target || !targetCount) {
        return 1;  /* Invalid arguments */
    }

    if (fseek(f, offset, SEEK_SET) != 0) {
        return 1;  /* Seek failed */
    }

    count = 0;
    currentIndex = -1;

    /* First pass: count number of shapes and collect sizes */
    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (*p == ' ' || *p == '\t') p++;

        if (*p == '\0' || *p == '#') continue;
        if (*p == '[') break;

        if (strncmp(p, "shape_id", 8) == 0) {
            count++;
        }
    }

    if (count == 0) {
        *target = NULL;
        *targetCount = 0;
        return 0;
    }

    /* Allocate array of shapes */
    shapes = (ShapeArbitrary*) ALLOC(sizeof(ShapeArbitrary) * count);
    if (!shapes) return 1;

    /* Reset file pointer for second part of init (still first pass) */
    if (fseek(f, offset, SEEK_SET) != 0) {
        FREE(shapes);
        return 1;
    }

    currentIndex = -1;

    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (*p == ' ' || *p == '\t') p++;

        if (*p == '\0' || *p == '#') continue;
        if (*p == '[') break;

        if (strncmp(p, "shape_id", 8) == 0) {
            currentIndex++;
            if (currentIndex >= count) break;

            shapes[currentIndex].numSamples = 0;
            shapes[currentIndex].numUncompressedSamples = 0;
            shapes[currentIndex].samples = NULL;
        }
        else if (strncmp(p, "num_samples", 11) == 0 && currentIndex >= 0) {
            if (sscanf(p + 11, "%d", &n) == 1) {
                shapes[currentIndex].numUncompressedSamples = n;
            }
        }
        else if (currentIndex >= 0) {
            shapes[currentIndex].numSamples++;
        }
    }

    /* Allocate sample arrays */
    for (currentIndex = 0; currentIndex < count; currentIndex++) {
        num = shapes[currentIndex].numSamples;
        shapes[currentIndex].samples = (float*) ALLOC(sizeof(float) * num);
        if (!shapes[currentIndex].samples) {
            for (i = 0; i < currentIndex; i++) {
                if (shapes[i].samples) FREE(shapes[i].samples);
            }
            FREE(shapes);
            return 1;
        }
    }

    *target = shapes;
    *targetCount = count;
    return 0;
}

int initRfShimLibrary(FILE* f, long offset, RfShimEntry** target, int* targetCount)
{
    char line[MAX_LINE_LENGTH];
    int maxIndex = -1;
    char* p;
    int idx, i;
    RfShimEntry* array;

    if (!f || !target || !targetCount) return 1;
    if (fseek(f, offset, SEEK_SET) != 0) return 1;

    /* Skip the section header line */
    if (!fgets(line, sizeof(line), f)) return 1;

    /* First pass: determine max index */
    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;
        if (sscanf(p, "%d", &idx) == 1) {
            if (idx > maxIndex) maxIndex = idx;
        }
    }

    if (maxIndex <= 0) {
        *target = NULL;
        *targetCount = 0;
        return 1; /* No entries found */
    }

    /* Allocate array of RfShimEntry */
    array = (RfShimEntry*) ALLOC(sizeof(RfShimEntry) * (maxIndex + 1));
    if (!array) return 1;

    for (i = 0; i <= maxIndex; i++) {
        array[i].nChannels = 0;
        array[i].values = NULL;
    }

    *target = array;
    *targetCount = maxIndex + 1;
    return 0;
}

int readStandardLibrary(FILE* f, long offset, void* target, int targetCount, int N, Scale scale, int flag)
{
    char line[MAX_LINE_LENGTH];
    int idx, parsed, consumed, n, offsetCol;                        
    float vals[MAX_SCALE_SIZE];
    char* scanPtr;
    char* p;
    float v;

    float *array_raw = (float*)target;
    if (!f) return 1;
    if (scale.size > MAX_SCALE_SIZE) return 1;
    if (fseek(f, offset, SEEK_SET) != 0) {
        return 1;
    }

    /* Skip section header line */
    if (!fgets(line, sizeof(line), f)) {
        return 1;
    }

    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[') break; /* next section */

        if (*p == '\0' || *p == '#') continue;
        if (sscanf(p, "%d", &idx) != 1) continue;
        if (idx < 0 || idx >= targetCount) continue;

        /* Move pointer past index */
        while (*p && *p != ' ' && *p != '\t') p++;
        while (*p == ' ' || *p == '\t') p++;

        parsed = 0;
        scanPtr = p;

        for (n = 0; n < scale.size; n++) {
            consumed = 0;
            if (sscanf(scanPtr, "%f%n", &v, &consumed) != 1) break;
            vals[n] = v;
            scanPtr += consumed;
            while (*scanPtr == ' ' || *scanPtr == '\t') scanPtr++;
            parsed++;
        }

        if (parsed != scale.size) continue;

        offsetCol = (flag >= 0) ? 1 : 0;
        for (n = 0; n < scale.size; n++) {
            array_raw[idx * N + n + offsetCol] = vals[n] * scale.values[n];
        }
        if (flag >= 0) {
            array_raw[idx * N + 0] = (float)flag;
        }
    }

    return 0;
}

int readLabelLibrary(FILE* f, long offset, void* target, int targetCount, int N, int* isLabelDefined) {
    char line[MAX_LINE_LENGTH];
    char* p;
    int idx, labelCode;
    float val;
    char label[LABEL_NAME_LENGTH];

    float *array_raw = (float*)target;
    if (!f || offset < 0) return 1;
    if (fseek(f, offset, SEEK_SET) != 0) return 1;

    /* Skip section header line */
    fgets(line, sizeof(line), f);

    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;
        if (sscanf(p, "%d %f %31s", &idx, &val, label) == 3 &&
            idx >= 0 && idx < targetCount) {

            labelCode = label2enum(label);
            
            /* bookkeep found labels and flags */
            if (labelCode > 0){
                isLabelDefined[labelCode] = 1;
            }

            array_raw[idx * N + 0] = val;
            array_raw[idx * N + 1] = (float)labelCode;
        }
    }

    return 0;
}

int readDelayLibrary(FILE* f, long offset, void* target, int targetCount, int N) {
    char line[MAX_LINE_LENGTH];
    char* p;
    int idx, hintCode;
    float offsetVal, scaleVal;
    char hint[SOFT_DELAY_HINT_LENGTH];

    float *array_raw = (float*)target;
    if (!f || offset < 0) return 1;
    if (fseek(f, offset, SEEK_SET) != 0) return 1;
    
    /* Skip section header line */
    fgets(line, sizeof(line), f);

    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;
        if (sscanf(p, "%d %f %f %31s", &idx, &offsetVal, &scaleVal, hint) == 4 &&
            idx >= 0 && idx < targetCount) {

            hintCode = hint2enum(hint);

            array_raw[idx * N + 0] = offsetVal;
            array_raw[idx * N + 1] = scaleVal;
            array_raw[idx * N + 2] = (float)hintCode;
        }
    }

    return 0;
}

int readRfShimLibrary(FILE* f, long offset, RfShimEntry* target, int targetCount)
{
    char line[MAX_LINE_LENGTH];
    char* p;
    int idx, nCh, i, consumed;
    float* values;
    float val;

    if (!f || !target) return 1;
    if (fseek(f, offset, SEEK_SET) != 0) return 1;

    /* Skip section header line */
    if (!fgets(line, sizeof(line), f)) return 1;

    while (fgets(line, sizeof(line), f)) {
        p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '[' || *p == '\0' || *p == '#') continue;
        if (sscanf(p, "%d %d", &idx, &nCh) != 2) continue;
        if (idx < 0 || idx >= targetCount || nCh <= 0) continue;

        /* Skip past the index and nCh */
        while (*p && *p != ' ') p++; while (*p == ' ') p++;
        while (*p && *p != ' ') p++; while (*p == ' ') p++;

        values = (float*) ALLOC(sizeof(float) * 2 * nCh);
        if (!values) return 1;

        for (i = 0; i < 2 * nCh; i++) {
            consumed = 0;
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