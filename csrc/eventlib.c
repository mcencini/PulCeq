/**
 * @file eventlib.c
 * @brief Implementation of EventLibraries parsing from Pulseq seqfile.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "constants.h"
#include "c_type.h"
#include "eventlib.h"
#include "seqfile.h"

void readAdcLibrary(SeqFile* seq)
{
    if (seq->isAdcLibraryParsed) return;

    Scale adcScale = { 
        .size = 8, 
        .values = (float[]){1, 1e-9f, 1e-6f, 1, 1, 1, 1, 1} 
    };

    int ret;
    ret = initStandardLibrary(seq->filePath, "[ADC]", 1, &seq->adcLibrary, &seq->adcLibrarySize, adcScale.size);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize adcLibrary\n");
        return;
    }
    ret = readStandardLibrary(seq->filePath, "[ADC]", seq->adcLibrary, seq->adcLibrarySize, adcScale, -1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read adcLibrary from file %s\n", seq->filePath);
        return;
    }

    seq->isAdcLibraryParsed = 1;
}

void readExtensionLibrary(SeqFile* seq)
{
    if (seq->isExtensionsLibraryParsed) return;

    Scale extScale = { 
        .size = 3, 
        .values = (float[]){ 1, 1, 1 } 
    };

    int ret;
    ret = initStandardLibrary(seq->filePath, "[EXTENSIONS]", 1, &seq->extensionsLibrary, &seq->extensionsLibrarySize, extScale.size);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize extensionsLibrary\n");
        return;
    }
    ret = readStandardLibrary(seq->filePath, "[EXTENSIONS]", seq->extensionsLibrary, seq->extensionsLibrarySize, extScale, -1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read extensionsLibrary from file %s\n", seq->filePath);
        return;
    }

    seq->isExtensionsLibraryParsed = 1;
}

void readGradLibrary(SeqFile* seq)
{
    if (seq->isGradLibraryParsed) return;

    const char* sections[] = { "[GRADIENTS]", "[TRAP]" };
    Scale gradScale = { 
        .size = 6, 
        .values = (float[]){ 1, 1, 1, 1, 1, 1e-6f } 
    };
    Scale trapScale = {
        .size = 5,
        .values = (float[]){ 1, 1e-6f, 1e-6f, 1e-6f, 1e-6f} 
    };

    int ret;
    ret = initStandardLibrary(seq->filePath, sections, 2, &seq->gradLibrary, &seq->gradLibrarySize, gradScale.size + 1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize gradLibrary\n");
        return;
    }
    ret = readStandardLibrary(seq->filePath, "[GRADIENTS]", seq->gradLibrary, seq->gradLibrarySize, gradScale, 1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read gradLibrary ([GRADIENTS] section) from file %s\n", seq->filePath);
        return;
    }
    ret = readStandardLibrary(seq->filePath, "[TRAP]", seq->gradLibrary, seq->gradLibrarySize, trapScale, 0);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read gradLibrary ([TRAP] section) from file %s\n", seq->filePath);
        return;
    }

    seq->isGradLibraryParsed = 1;
}

void readRfLibrary(SeqFile* seq)
{
    if (seq->isRfLibraryParsed) return;

    const char* sections[] = { "[RF]" };
    Scale rfScale = {
        .size = 10,
        .values = (float[]) {1, 1, 1, 1, 1e-6f, 1e-6f, 1, 1, 1, 1}
    };

    int ret;
    ret = initStandardLibrary(seq->filePath, sections, 1, &seq->rfLibrary, &seq->rfLibrarySize, rfScale.size);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to initialize rfLibrary\n");
        return;
    }
    ret = readStandardLibrary(seq->filePath, "[RF]", seq->rfLibrary, seq->rfLibrarySize, rfScale, -1);
    if (ret != 0) {
        fprintf(stderr, "Error: Failed to read rfLibrary from file %s\n", seq->filePath);
        return;
    }

    seq->isRfLibraryParsed = 1;
}

/*************************Local utils  ****************************************/
/**
 * @brief Scale struct used for multiplying parsed library values.
 */
typedef struct {
    int size;         /**< Number of values to scale */
    const float* values; /**< Array of scaling factors */
} Scale;

/**
 * @brief Initialize a standard library array for given sections.
 *
 * Scans the file for all given section names, finds the maximum index in
 * the first column of each section, then allocates and zero-fills a float** array.
 * The array size will be (maxIndex + 1) rows, each with numEntries floats.
 *
 * @param[in] filename      Input file name
 * @param[in] sectionNames  Array of section header strings (e.g. "[GRAD]", "[TRAP]")
 * @param[in] numSections   Number of section names
 * @param[out] target       Pointer to float** pointer to store allocated array
 * @param[out] targetCount  Pointer to int to store number of allocated rows
 * @param[in] numEntries    Number of entries per row (columns)
 * 
 * @return 0 on success, non-zero on failure
 */
int initStandardLibrary(const char* filename, const char** sectionNames, int numSections, float*** target, int* targetCount, int numEntries)
{
    FILE* f = fopen(filename, "r");
    if (!f) return 1;

    char line[MAX_LINE_LENGTH];
    int maxIndex = -1;
    int inSection = 0;

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (is_space((unsigned char)*p)) p++;

        if (*p == '\0' || *p == '#') continue;

        /* Check if line is a section header */
        if (*p == '[') {
            inSection = 0;
            for (int i = 0; i < numSections; i++) {
                if (strncmp(p, sectionNames[i], strlen(sectionNames[i])) == 0) {
                    inSection = 1;
                    break;
                }
            }
            continue;
        }

        if (!inSection) continue;

        /* Parse index from first integer */
        int idx;
        if (sscanf(p, "%d", &idx) == 1) {
            if (idx > maxIndex) maxIndex = idx;
        }
    }

    fclose(f);

    if (maxIndex < 0) {
        *target = NULL;
        *targetCount = 0;
        return 2; /* No entries found */
    }

    /* Allocate zero-filled array */
    float** array = (float**) ALLOC(sizeof(float*) * (maxIndex + 1));
    if (!array) return 3;

    for (int i = 0; i <= maxIndex; i++) {
        array[i] = (float*) ALLOC(numEntries * sizeof(float));
        if (!array[i]) {
            for (int j = 0; j < i; j++) FREE(array[j]);
            FREE(array);
            return 3;
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
 * @brief Read a standard library section and fill a pre-allocated array.
 *
 * Parses each valid line (index followed by values), applies scaling,
 * and stores scaled values in the target array at the row corresponding to the index.
 * If 'flag' >= 0, sets target[index][0] = flag (used for GRAD/TRAP libraries).
 *
 * @param[in] filename     Input file name
 * @param[in] sectionName  Section header string (e.g. "[GRAD]")
 * @param[in,out] target   Pre-allocated 2D float array (rows x scale.size [+1 if flag used])
 * @param[in] targetCount  Number of allocated rows
 * @param[in] scale        Scale struct for scaling values
 * @param[in] flag         If >= 0, sets target[index][0] to this value
 * 
 * @return 0 on success, non-zero on failure
 */
int readStandardLibrary(const char* filename, const char* sectionName, float** target, int targetCount, Scale scale, int flag)
{
    FILE* f = fopen(filename, "r");
    if (!f) return 1;

    char line[MAX_LINE_LENGTH];
    int inSection = 0;

    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (is_space((unsigned char)*p)) p++;

        if (*p == '\0' || *p == '#') continue;

        if (!inSection) {
            if (strncmp(p, sectionName, strlen(sectionName)) == 0) {
                inSection = 1;
            }
            continue;
        } else if (*p == '[') {
            break; /* Next section */
        }

        /* Parse index */
        int idx;
        if (sscanf(p, "%d", &idx) != 1 || idx < 0 || idx >= targetCount) continue;

        /* Move to values after index */
        while (*p && !is_space((unsigned char)*p)) p++;
        while (is_space((unsigned char)*p)) p++;

        /* Parse and scale values */
        float vals[scale.size];
        int parsed = 0;
        char* scanPtr = p;

        for (int n = 0; n < scale.size; n++) {
            float v;
            int consumed;
            if (sscanf(scanPtr, "%f%n", &v, &consumed) != 1) break;
            vals[n] = v;
            scanPtr += consumed;
            while (is_space((unsigned char)*scanPtr)) scanPtr++;
            parsed++;
        }

        if (parsed != scale.size) continue;

        /* Store values */
        int offset = (flag >= 0) ? 1 : 0;
        for (int n = 0; n < scale.size; n++) {
            target[idx][n + offset] = vals[n] * scale.values[n];
        }

        if (flag >= 0) {
            target[idx][0] = (float)flag;
        }
    }

    fclose(f);
    return 0;
}
