/**
 * @file readlib.c
 * @brief Implementation of common EventLibrary reading subroutines.
 *
 */

#include "readlib.h"

int initStandardLibrary(FILE* f, const long* offsets, int numSections, float*** target, int* targetCount, int numEntries)
{
    if (!f) return 1;

    char line[MAX_LINE_LENGTH];
    int maxIndex = -1;

    for (int sec = 0; sec < numSections; sec++) {
        if (offsets[sec] < 0) continue;  /* skip not found */

        if (fseek(f, offsets[sec], SEEK_SET) != 0) {
            fclose(f);
            return 2;
        }

        /* Skip the section header line */
        if (!fgets(line, sizeof(line), f)) {
            fclose(f);
            return 3;
        }

        /* Read until next section or EOF */
        while (fgets(line, sizeof(line), f)) {
            char* p = line;
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '[') break; /* Next section starts */

            if (*p == '\0' || *p == '#') continue; /* skip blank/comment */

            int idx = -1;
            if (sscanf(p, "%d", &idx) == 1) {
                if (idx > maxIndex) maxIndex = idx;
            }
        }
    }

    if (maxIndex < 0) {
        *target = NULL;
        *targetCount = 0;
        return 4; /* no entries found */
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

/**
 * @brief Read and parse one standard library section from file at given offset.
 *
 * Reads lines starting at offset until next section or EOF.
 * Parses index and values, scales values, and stores in target array.
 * If flag >= 0, sets target[index][0] = flag.
 *
 * @param[in] f              Opened file handle (text mode)
 * @param[in] offset         File offset where section starts.
 * @param[in,out] target     Pre-allocated 2D float array.
 * @param[in] targetCount    Number of rows in target.
 * @param[in] scale          Scale struct with size and values.
 * @param[in] flag           Flag to store at target[index][0], or -1 to ignore.
 *
 * @return 0 on success, non-zero on failure.
 */
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