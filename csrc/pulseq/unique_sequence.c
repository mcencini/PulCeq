/**
 * @file unique_sequence.c
 * @brief Implementation of functions for creating unique sequences.
 */

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "unique_sequence.h"
#include "alloc.h"
#include "seqfile.h"

/* Define library size constants */
#define RF_COLS 10
#define GRAD_COLS 7
#define ADC_COLS 8
#define BLOCK_COLS 7

/* Comparison functions for qsort */

/**
 * @brief Compare function for RF events.
 */
static int compareRFEvents(const void* a, const void* b) {
    const int* indexA = (const int*)a;
    const int* indexB = (const int*)b;
    const float* matrixA = *(const float**)a; // Get the matrix entry for index A
    const float* matrixB = *(const float**)b; // Get the matrix entry for index B
    
    /* Compare mag_id, phase_id, time_id, and delay */
    if (matrixA[0] != matrixB[0]) return matrixA[0] < matrixB[0] ? -1 : 1;  /* mag_id */
    if (matrixA[1] != matrixB[1]) return matrixA[1] < matrixB[1] ? -1 : 1;  /* phase_id */
    if (matrixA[2] != matrixB[2]) return matrixA[2] < matrixB[2] ? -1 : 1;  /* time_id */
    
    return 0;  /* Events are identical */
}

/**
 * @brief Compare function for gradient events.
 */
static int compareGradEvents(const void* a, const void* b) {
    const int* indexA = (const int*)a;
    const int* indexB = (const int*)b;
    const float* matrixA = *(const float**)a;
    const float* matrixB = *(const float**)b;
    
    /* First check if types are different */
    if (matrixA[0] != matrixB[0]) return matrixA[0] < matrixB[0] ? -1 : 1;  /* type */
    
    if (matrixA[0] == 0) {  /* Trapezoid */
        /* Compare rise, flat, fall, and delay */
        if (matrixA[1] != matrixB[1]) return matrixA[1] < matrixB[1] ? -1 : 1;  /* rise */
        if (matrixA[2] != matrixB[2]) return matrixA[2] < matrixB[2] ? -1 : 1;  /* flat */
        if (matrixA[3] != matrixB[3]) return matrixA[3] < matrixB[3] ? -1 : 1;  /* fall */
        if (matrixA[4] != matrixB[4]) return matrixA[4] < matrixB[4] ? -1 : 1;  /* delay */
    } else {  /* Arbitrary */
        /* Compare shape_id, first, last, time_id, and delay */
        if (matrixA[1] != matrixB[1]) return matrixA[1] < matrixB[1] ? -1 : 1;  /* first */
        if (matrixA[2] != matrixB[2]) return matrixA[2] < matrixB[2] ? -1 : 1;  /* last */
        if (matrixA[3] != matrixB[3]) return matrixA[3] < matrixB[3] ? -1 : 1;  /* shape_id */
        if (matrixA[4] != matrixB[4]) return matrixA[4] < matrixB[4] ? -1 : 1;  /* delay */
    }
    
    return 0;  /* Events are identical */
}

/**
 * @brief Compare function for ADC events.
 */
static int compareADCEvents(const void* a, const void* b) {
    const int* indexA = (const int*)a;
    const int* indexB = (const int*)b;
    const float* matrixA = *(const float**)a;
    const float* matrixB = *(const float**)b;
    
    /* Compare num, dwell, delay, and phase_id */
    if (matrixA[0] != matrixB[0]) return matrixA[0] < matrixB[0] ? -1 : 1;  /* num */
    if (matrixA[1] != matrixB[1]) return matrixA[1] < matrixB[1] ? -1 : 1;  /* dwell */
    if (matrixA[2] != matrixB[2]) return matrixA[2] < matrixB[2] ? -1 : 1;  /* delay */
    
    return 0;  /* Events are identical */
}

/**
 * @brief Find unique RF events in the sequence.
 * 
 * @param seq Source sequence file
 * @param rfMap Output mapping from original indices to unique indices (1-based)
 * @param uniqueRfCount Output pointer to store number of unique RF events
 * @param uniqueRfLibrary Output array for unique RF events
 */
static void findUniqueRF(const SeqFile* seq, int* rfMap, int* uniqueRfCount, float (*uniqueRfLibrary)[RF_COLS]) {
    int n;
    int i, j, uniqueIndex;
    int *sortedIndices;
    float **rfMatrix;
    int *firstAppearance;
    float temp[RF_COLS];
    int tempIndex;

    n = seq->rfLibrarySize;
    *uniqueRfCount = 0;
    if (n == 0) return;

    /* Allocate memory for sorted indices, RF comparison matrix, and first appearance tracking */
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    rfMatrix = (float**)ALLOC(n * sizeof(float*));
    firstAppearance = (int*)ALLOC(n * sizeof(int)); /* Track first appearance of unique events */

    if (!sortedIndices || !rfMatrix || !firstAppearance) {
        if (sortedIndices) FREE(sortedIndices);
        if (rfMatrix) FREE(rfMatrix);
        if (firstAppearance) FREE(firstAppearance);
        return;
    }

    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        rfMatrix[i] = (float*)ALLOC(3 * sizeof(float));
        if (!rfMatrix[i]) {
            for (j = 0; j < i; j++) {
                FREE(rfMatrix[j]);
            }
            FREE(rfMatrix);
            FREE(sortedIndices);
            FREE(firstAppearance);
            return;
        }

        rfMatrix[i][0] = seq->rfLibrary[i][1]; /* mag_id */
        rfMatrix[i][1] = seq->rfLibrary[i][2]; /* phase_id */
        rfMatrix[i][2] = seq->rfLibrary[i][3]; /* time_id */
        sortedIndices[i] = i;
    }

    /* Sort using qsort */
    qsort(sortedIndices, n, sizeof(int), compareRFEvents);

    /* Identify unique events and track their first appearance */
    *uniqueRfCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareRFEvents(&sortedIndices[i], &sortedIndices[i - 1]) != 0) {
            /* Copy the entire row from the original library */
            memcpy(uniqueRfLibrary[*uniqueRfCount], seq->rfLibrary[sortedIndices[i]], RF_COLS * sizeof(float));
            firstAppearance[*uniqueRfCount] = sortedIndices[i]; /* Track first appearance */
            (*uniqueRfCount)++;
        }
        /* Map from original indices to unique indices (1-based) */
        rfMap[sortedIndices[i]] = *uniqueRfCount;
    }

    /* Sort unique events by their first appearance */
    for (i = 0; i < *uniqueRfCount - 1; i++) {
        for (j = i + 1; j < *uniqueRfCount; j++) {
            if (firstAppearance[i] > firstAppearance[j]) {
                /* Swap entries in uniqueRfLibrary */
                memcpy(temp, uniqueRfLibrary[i], RF_COLS * sizeof(float));
                memcpy(uniqueRfLibrary[i], uniqueRfLibrary[j], RF_COLS * sizeof(float));
                memcpy(uniqueRfLibrary[j], temp, RF_COLS * sizeof(float));

                /* Swap entries in firstAppearance */
                tempIndex = firstAppearance[i];
                firstAppearance[i] = firstAppearance[j];
                firstAppearance[j] = tempIndex;
            }
        }
    }

    /* Update rfMap to reflect the new order of unique events */
    for (i = 0; i < n; i++) {
        for (uniqueIndex = 0; uniqueIndex < *uniqueRfCount; uniqueIndex++) {
            if (sortedIndices[i] == firstAppearance[uniqueIndex]) {
                rfMap[sortedIndices[i]] = uniqueIndex + 1; /* 1-based indexing */
                break;
            }
        }
    }

    /* Free temporary memory */
    for (i = 0; i < n; i++) {
        FREE(rfMatrix[i]);
    }
    FREE(rfMatrix);
    FREE(sortedIndices);
    FREE(firstAppearance);
}

/**
 * @brief Find unique gradient events in the sequence.
 * 
 * @param seq Source sequence file
 * @param gradMap Output mapping from original indices to unique indices (1-based)
 * @param uniqueGradCount Output pointer to store number of unique gradient events
 * @param uniqueGradLibrary Output array for unique gradient events
 */
static void findUniqueGrad(const SeqFile* seq, int* gradMap, int* uniqueGradCount, float (*uniqueGradLibrary)[GRAD_COLS]) {
    int n;
    int i, j, uniqueIndex;
    int *sortedIndices;
    float **gradMatrix;
    int *firstAppearance;
    float temp[GRAD_COLS];
    int tempIndex;

    n = seq->gradLibrarySize;
    *uniqueGradCount = 0;
    if (n == 0) return;

    /* Allocate memory for sorted indices, gradient comparison matrix, and first appearance tracking */
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    gradMatrix = (float**)ALLOC(n * sizeof(float*));
    firstAppearance = (int*)ALLOC(n * sizeof(int)); /* Track first appearance of unique events */

    if (!sortedIndices || !gradMatrix || !firstAppearance) {
        if (sortedIndices) FREE(sortedIndices);
        if (gradMatrix) FREE(gradMatrix);
        if (firstAppearance) FREE(firstAppearance);
        return;
    }

    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        gradMatrix[i] = (float*)ALLOC(5 * sizeof(float));
        if (!gradMatrix[i]) {
            for (j = 0; j < i; j++) {
                FREE(gradMatrix[j]);
            }
            FREE(gradMatrix);
            FREE(sortedIndices);
            FREE(firstAppearance);
            return;
        }

        gradMatrix[i][0] = seq->gradLibrary[i][0]; /* type */
        gradMatrix[i][1] = seq->gradLibrary[i][2]; /* first */
        gradMatrix[i][2] = seq->gradLibrary[i][3]; /* last */
        gradMatrix[i][3] = seq->gradLibrary[i][4]; /* shape_id */
        gradMatrix[i][4] = seq->gradLibrary[i][6]; /* delay */
        sortedIndices[i] = i;
    }

    /* Sort using qsort */
    qsort(sortedIndices, n, sizeof(int), compareGradEvents);

    /* Identify unique events and track their first appearance */
    *uniqueGradCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareGradEvents(&sortedIndices[i], &sortedIndices[i - 1]) != 0) {
            /* Copy the entire row from the original library */
            memcpy(uniqueGradLibrary[*uniqueGradCount], seq->gradLibrary[sortedIndices[i]], GRAD_COLS * sizeof(float));
            firstAppearance[*uniqueGradCount] = sortedIndices[i]; /* Track first appearance */
            (*uniqueGradCount)++;
        }
        /* Map from original indices to unique indices (1-based) */
        gradMap[sortedIndices[i]] = *uniqueGradCount;
    }

    /* Sort unique events by their first appearance */
    for (i = 0; i < *uniqueGradCount - 1; i++) {
        for (j = i + 1; j < *uniqueGradCount; j++) {
            if (firstAppearance[i] > firstAppearance[j]) {
                /* Swap entries in uniqueGradLibrary */
                memcpy(temp, uniqueGradLibrary[i], GRAD_COLS * sizeof(float));
                memcpy(uniqueGradLibrary[i], uniqueGradLibrary[j], GRAD_COLS * sizeof(float));
                memcpy(uniqueGradLibrary[j], temp, GRAD_COLS * sizeof(float));

                /* Swap entries in firstAppearance */
                tempIndex = firstAppearance[i];
                firstAppearance[i] = firstAppearance[j];
                firstAppearance[j] = tempIndex;
            }
        }
    }

    /* Update gradMap to reflect the new order of unique events */
    for (i = 0; i < n; i++) {
        for (uniqueIndex = 0; uniqueIndex < *uniqueGradCount; uniqueIndex++) {
            if (sortedIndices[i] == firstAppearance[uniqueIndex]) {
                gradMap[sortedIndices[i]] = uniqueIndex + 1; /* 1-based indexing */
                break;
            }
        }
    }

    /* Free temporary memory */
    for (i = 0; i < n; i++) {
        FREE(gradMatrix[i]);
    }
    FREE(gradMatrix);
    FREE(sortedIndices);
    FREE(firstAppearance);
}

/**
 * @brief Find unique ADC events in the sequence.
 * 
 * @param seq Source sequence file
 * @param adcMap Output mapping from original indices to unique indices (1-based)
 * @param uniqueAdcCount Output pointer to store number of unique ADC events
 * @param uniqueAdcLibrary Output array for unique ADC events
 */
static void findUniqueADC(const SeqFile* seq, int* adcMap, int* uniqueAdcCount, float (*uniqueAdcLibrary)[ADC_COLS]) {
    int n;
    int i, j, uniqueIndex;
    int *sortedIndices;
    float **adcMatrix;
    int *firstAppearance;
    float temp[ADC_COLS];
    int tempIndex;

    n = seq->adcLibrarySize;
    *uniqueAdcCount = 0;
    if (n == 0) return;

    /* Allocate memory for sorted indices, ADC comparison matrix, and first appearance tracking */
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    adcMatrix = (float**)ALLOC(n * sizeof(float*));
    firstAppearance = (int*)ALLOC(n * sizeof(int)); /* Track first appearance of unique events */

    if (!sortedIndices || !adcMatrix || !firstAppearance) {
        if (sortedIndices) FREE(sortedIndices);
        if (adcMatrix) FREE(adcMatrix);
        if (firstAppearance) FREE(firstAppearance);
        return;
    }

    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        adcMatrix[i] = (float*)ALLOC(3 * sizeof(float));
        if (!adcMatrix[i]) {
            for (j = 0; j < i; j++) {
                FREE(adcMatrix[j]);
            }
            FREE(adcMatrix);
            FREE(sortedIndices);
            FREE(firstAppearance);
            return;
        }

        adcMatrix[i][0] = seq->adcLibrary[i][0]; /* num */
        adcMatrix[i][1] = seq->adcLibrary[i][1]; /* dwell */
        adcMatrix[i][2] = seq->adcLibrary[i][2]; /* delay */
        sortedIndices[i] = i;
    }

    /* Sort using qsort */
    qsort(sortedIndices, n, sizeof(int), compareADCEvents);

    /* Identify unique events and track their first appearance */
    *uniqueAdcCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareADCEvents(&sortedIndices[i], &sortedIndices[i - 1]) != 0) {
            /* Copy the entire row from the original library */
            memcpy(uniqueAdcLibrary[*uniqueAdcCount], seq->adcLibrary[sortedIndices[i]], ADC_COLS * sizeof(float));
            firstAppearance[*uniqueAdcCount] = sortedIndices[i]; /* Track first appearance */
            (*uniqueAdcCount)++;
        }
        /* Map from original indices to unique indices (1-based) */
        adcMap[sortedIndices[i]] = *uniqueAdcCount;
    }

    /* Sort unique events by their first appearance */
    for (i = 0; i < *uniqueAdcCount - 1; i++) {
        for (j = i + 1; j < *uniqueAdcCount; j++) {
            if (firstAppearance[i] > firstAppearance[j]) {
                /* Swap entries in uniqueAdcLibrary */
                memcpy(temp, uniqueAdcLibrary[i], ADC_COLS * sizeof(float));
                memcpy(uniqueAdcLibrary[i], uniqueAdcLibrary[j], ADC_COLS * sizeof(float));
                memcpy(uniqueAdcLibrary[j], temp, ADC_COLS * sizeof(float));

                /* Swap entries in firstAppearance */
                tempIndex = firstAppearance[i];
                firstAppearance[i] = firstAppearance[j];
                firstAppearance[j] = tempIndex;
            }
        }
    }

    /* Update adcMap to reflect the new order of unique events */
    for (i = 0; i < n; i++) {
        for (uniqueIndex = 0; uniqueIndex < *uniqueAdcCount; uniqueIndex++) {
            if (sortedIndices[i] == firstAppearance[uniqueIndex]) {
                adcMap[sortedIndices[i]] = uniqueIndex + 1; /* 1-based indexing */
                break;
            }
        }
    }

    /* Free temporary memory */
    for (i = 0; i < n; i++) {
        FREE(adcMatrix[i]);
    }
    FREE(adcMatrix);
    FREE(sortedIndices);
    FREE(firstAppearance);
}

/**
 * @brief Create a new sequence with unique RF, gradient, and ADC events.
 * 
 * @param uniqueSeq Pointer to the output SeqFile structure for the unique sequence.
 * @param seq Pointer to the input sequence file.
 * @return int 1 if successful, 0 if failed.
 */
int uniqueSequence(SeqFile* uniqueSeq, const SeqFile* seq) {
    /* All declarations at top for ANSI C89 compliance */
    int i, j;
    int *rfMap;
    int *gradXMap;
    int *gradYMap;
    int *gradZMap;
    int *adcMap;
    int uniqueRfCount;
    int uniqueGradXCount;
    int uniqueGradYCount;
    int uniqueGradZCount;
    int uniqueAdcCount;
    float (*uniqueRfLibrary)[RF_COLS];
    float (*uniqueGradXLibrary)[GRAD_COLS];
    float (*uniqueGradYLibrary)[GRAD_COLS];
    float (*uniqueGradZLibrary)[GRAD_COLS];
    float (*uniqueAdcLibrary)[ADC_COLS];

    /* Initialize all pointers to NULL and counts to 0 */
    rfMap = gradXMap = gradYMap = gradZMap = adcMap = NULL;
    uniqueRfCount = uniqueGradXCount = uniqueGradYCount = uniqueGradZCount = uniqueAdcCount = 0;
    uniqueRfLibrary = NULL;
    uniqueGradXLibrary = NULL;
    uniqueGradYLibrary = NULL;
    uniqueGradZLibrary = NULL;
    uniqueAdcLibrary = NULL;

    /* Initialize the output sequence */
    __seqFileReset(uniqueSeq);

    /* Only keep blockLibrary, rfLibrary, gradLibrary, adcLibrary, and blockIDs in output */
    /* If blockIDs is present in struct, set to NULL. Otherwise, leave untouched. */

    /* Allocate temporary arrays to store the mapping between original and unique events */
    if (seq->rfLibrarySize > 0) {
        rfMap = (int*)ALLOC(seq->rfLibrarySize * sizeof(int));
        if (!rfMap) return 0;
    }
    if (seq->gradLibrarySize > 0) {
        gradXMap = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
        gradYMap = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
        gradZMap = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
        if (!gradXMap || !gradYMap || !gradZMap) {
            if (rfMap) FREE(rfMap);
            if (gradXMap) FREE(gradXMap);
            if (gradYMap) FREE(gradYMap);
            if (gradZMap) FREE(gradZMap);
            return 0;
        }
    }
    if (seq->adcLibrarySize > 0) {
        adcMap = (int*)ALLOC(seq->adcLibrarySize * sizeof(int));
        if (!adcMap) {
            if (rfMap) FREE(rfMap);
            if (gradXMap) FREE(gradXMap);
            if (gradYMap) FREE(gradYMap);
            if (gradZMap) FREE(gradZMap);
            return 0;
        }
    }

    /* Find unique events for each library */
    if (seq->rfLibrarySize > 0) {
        uniqueRfLibrary = (float (*)[RF_COLS])ALLOC(seq->rfLibrarySize * sizeof(float[RF_COLS]));
        if (!uniqueRfLibrary) {
            if (rfMap) FREE(rfMap);
            if (gradXMap) FREE(gradXMap);
            if (gradYMap) FREE(gradYMap);
            if (gradZMap) FREE(gradZMap);
            if (adcMap) FREE(adcMap);
            return 0;
        }
        findUniqueRF(seq, rfMap, &uniqueRfCount, uniqueRfLibrary);
        uniqueSeq->rfLibrarySize = uniqueRfCount;
        uniqueSeq->rfLibrary = uniqueRfLibrary;
        uniqueSeq->isRfLibraryParsed = 1;
    }
    if (seq->gradLibrarySize > 0) {
        uniqueGradXLibrary = (float (*)[GRAD_COLS])ALLOC(seq->gradLibrarySize * sizeof(float[GRAD_COLS]));
        uniqueGradYLibrary = (float (*)[GRAD_COLS])ALLOC(seq->gradLibrarySize * sizeof(float[GRAD_COLS]));
        uniqueGradZLibrary = (float (*)[GRAD_COLS])ALLOC(seq->gradLibrarySize * sizeof(float[GRAD_COLS]));
        if (!uniqueGradXLibrary || !uniqueGradYLibrary || !uniqueGradZLibrary) {
            if (rfMap) FREE(rfMap);
            if (gradXMap) FREE(gradXMap);
            if (gradYMap) FREE(gradYMap);
            if (gradZMap) FREE(gradZMap);
            if (adcMap) FREE(adcMap);
            if (uniqueRfLibrary) FREE(uniqueRfLibrary);
            if (uniqueGradXLibrary) FREE(uniqueGradXLibrary);
            if (uniqueGradYLibrary) FREE(uniqueGradYLibrary);
            if (uniqueGradZLibrary) FREE(uniqueGradZLibrary);
            return 0;
        }
        findUniqueGrad(seq, gradXMap, &uniqueGradXCount, uniqueGradXLibrary);
        findUniqueGrad(seq, gradYMap, &uniqueGradYCount, uniqueGradYLibrary);
        findUniqueGrad(seq, gradZMap, &uniqueGradZCount, uniqueGradZLibrary);
        uniqueSeq->gradLibrarySize = uniqueGradXCount + uniqueGradYCount + uniqueGradZCount;
        uniqueSeq->gradLibrary = (float(*)[GRAD_COLS])ALLOC(uniqueSeq->gradLibrarySize * sizeof(float[GRAD_COLS]));
        if (!uniqueSeq->gradLibrary) {
            if (rfMap) FREE(rfMap);
            if (gradXMap) FREE(gradXMap);
            if (gradYMap) FREE(gradYMap);
            if (gradZMap) FREE(gradZMap);
            if (adcMap) FREE(adcMap);
            if (uniqueRfLibrary) FREE(uniqueRfLibrary);
            if (uniqueGradXLibrary) FREE(uniqueGradXLibrary);
            if (uniqueGradYLibrary) FREE(uniqueGradYLibrary);
            if (uniqueGradZLibrary) FREE(uniqueGradZLibrary);
            return 0;
        }
        for (i = 0; i < uniqueGradXCount; i++) {
            memcpy(uniqueSeq->gradLibrary[i], uniqueGradXLibrary[i], GRAD_COLS * sizeof(float));
        }
        for (i = 0; i < uniqueGradYCount; i++) {
            memcpy(uniqueSeq->gradLibrary[uniqueGradXCount + i], uniqueGradYLibrary[i], GRAD_COLS * sizeof(float));
        }
        for (i = 0; i < uniqueGradZCount; i++) {
            memcpy(uniqueSeq->gradLibrary[uniqueGradXCount + uniqueGradYCount + i], uniqueGradZLibrary[i], GRAD_COLS * sizeof(float));
        }
        for (i = 0; i < seq->gradLibrarySize; i++) {
            if (gradYMap[i] > 0) gradYMap[i] += uniqueGradXCount;
            if (gradZMap[i] > 0) gradZMap[i] += (uniqueGradXCount + uniqueGradYCount);
        }
        uniqueSeq->isGradLibraryParsed = 1;
        FREE(uniqueGradXLibrary);
        FREE(uniqueGradYLibrary);
        FREE(uniqueGradZLibrary);
        uniqueGradXLibrary = uniqueGradYLibrary = uniqueGradZLibrary = NULL;
    }
    if (seq->adcLibrarySize > 0) {
        uniqueAdcLibrary = (float (*)[ADC_COLS])ALLOC(seq->adcLibrarySize * sizeof(float[ADC_COLS]));
        if (!uniqueAdcLibrary) {
            if (rfMap) FREE(rfMap);
            if (gradXMap) FREE(gradXMap);
            if (gradYMap) FREE(gradYMap);
            if (gradZMap) FREE(gradZMap);
            if (adcMap) FREE(adcMap);
            if (uniqueRfLibrary) FREE(uniqueRfLibrary);
            if (uniqueSeq->gradLibrary) FREE(uniqueSeq->gradLibrary);
            return 0;
        }
        findUniqueADC(seq, adcMap, &uniqueAdcCount, uniqueAdcLibrary);
        uniqueSeq->adcLibrarySize = uniqueAdcCount;
        uniqueSeq->adcLibrary = uniqueAdcLibrary;
        uniqueSeq->isAdcLibraryParsed = 1;
    }
    if (seq->numBlocks > 0) {
        uniqueSeq->numBlocks = seq->numBlocks;
        uniqueSeq->blockLibrary = (float(*)[7])ALLOC(uniqueSeq->numBlocks * sizeof(float[7]));
        if (!uniqueSeq->blockLibrary) {
            if (rfMap) FREE(rfMap);
            if (gradXMap) FREE(gradXMap);
            if (gradYMap) FREE(gradYMap);
            if (gradZMap) FREE(gradZMap);
            if (adcMap) FREE(adcMap);
            if (uniqueRfLibrary) FREE(uniqueRfLibrary);
            if (uniqueSeq->gradLibrary) FREE(uniqueSeq->gradLibrary);
            if (uniqueAdcLibrary) FREE(uniqueAdcLibrary);
            return 0;
        }
        for (i = 0; i < seq->numBlocks; i++) {
            uniqueSeq->blockLibrary[i][0] = seq->blockLibrary[i][0];
            j = (int)seq->blockLibrary[i][1];
            uniqueSeq->blockLibrary[i][1] = j > 0 ? rfMap[j - 1] : 0;
            j = (int)seq->blockLibrary[i][2];
            uniqueSeq->blockLibrary[i][2] = j > 0 ? gradXMap[j - 1] : 0;
            j = (int)seq->blockLibrary[i][3];
            uniqueSeq->blockLibrary[i][3] = j > 0 ? gradYMap[j - 1] : 0;
            j = (int)seq->blockLibrary[i][4];
            uniqueSeq->blockLibrary[i][4] = j > 0 ? gradZMap[j - 1] : 0;
            j = (int)seq->blockLibrary[i][5];
            uniqueSeq->blockLibrary[i][5] = j > 0 ? adcMap[j - 1] : 0;
            uniqueSeq->blockLibrary[i][6] = 0;
        }
        uniqueSeq->isBlockLibraryParsed = 1;
    }
    /* blockIDs mapping can be filled here if needed */
    /* All other libraries and metadata are left NULL/0 */
    if (rfMap) FREE(rfMap);
    if (gradXMap) FREE(gradXMap);
    if (gradYMap) FREE(gradYMap);
    if (gradZMap) FREE(gradZMap);
    if (adcMap) FREE(adcMap);
    return 1;
}

/**
 * @brief Comparison function for block rows.
 * 
 * @param[in] a Pointer to first block row.
 * @param[in] b Pointer to second block row.
 * @return -1 if a < b, 1 if a > b, 0 if equal.
 */
static int compareBlockRows(const void* a, const void* b) {
    const float* fa = (const float*)a;
    const float* fb = (const float*)b;
    int i;
    
    /* Compare all columns in the block */
    for (i = 0; i < BLOCK_COLS; i++) {
        if (fa[i] < fb[i]) return -1;
        if (fa[i] > fb[i]) return 1;
    }
    return 0;
}

/**
 * @brief Get an array mapping original block IDs to unique block IDs.
 * 
 * @param seq Pointer to the sequence file.
 * @param[out] numUniqueBlocks Pointer to store the number of unique blocks.
 * @return int* Array mapping original block IDs to unique block IDs, NULL if failed.
 *         Caller is responsible for freeing this memory.
 */
int getUniqueBlockIDs(SeqFile* uniqueSeq, const SeqFile* seq, int* numUniqueBlocks) {
    int n;
    int i, j, uniqueIndex;
    int *sortedIndices;
    float (*blockMatrix)[BLOCK_COLS];
    int *firstAppearance;
    float temp[BLOCK_COLS];
    int tempIndex;

    n = seq->numBlocks;
    *numUniqueBlocks = 0;
    if (n == 0) return 0;

    /* Allocate memory for the block matrix, sorted indices, and first appearance tracking */
    blockMatrix = (float (*)[BLOCK_COLS])ALLOC(n * sizeof(float[BLOCK_COLS]));
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    firstAppearance = (int*)ALLOC(n * sizeof(int));
    uniqueSeq->blockIDs = (int*)ALLOC(n * sizeof(int)); /* Allocate blockIDs directly in uniqueSeq */

    if (!blockMatrix || !sortedIndices || !firstAppearance || !uniqueSeq->blockIDs) {
        if (blockMatrix) FREE(blockMatrix);
        if (sortedIndices) FREE(sortedIndices);
        if (firstAppearance) FREE(firstAppearance);
        if (uniqueSeq->blockIDs) FREE(uniqueSeq->blockIDs);
        return 0;
    }

    /* Copy block data to the matrix */
    for (i = 0; i < n; i++) {
        memcpy(blockMatrix[i], seq->blockLibrary[i], BLOCK_COLS * sizeof(float));
        sortedIndices[i] = i;
    }

    /* Sort using qsort */
    qsort(sortedIndices, n, sizeof(int), compareBlockRows);

    /* Identify unique blocks and track their first appearance */
    *numUniqueBlocks = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareBlockRows(blockMatrix[sortedIndices[i]], blockMatrix[sortedIndices[i - 1]]) != 0) {
            firstAppearance[*numUniqueBlocks] = sortedIndices[i]; /* Track first appearance */
            (*numUniqueBlocks)++;
        }
        /* Map from original indices to unique indices (1-based) */
        uniqueSeq->blockIDs[sortedIndices[i]] = *numUniqueBlocks;
    }

    /* Sort unique blocks by their first appearance */
    for (i = 0; i < *numUniqueBlocks - 1; i++) {
        for (j = i + 1; j < *numUniqueBlocks; j++) {
            if (firstAppearance[i] > firstAppearance[j]) {
                /* Swap entries in blockMatrix */
                memcpy(temp, blockMatrix[firstAppearance[i]], BLOCK_COLS * sizeof(float));
                memcpy(blockMatrix[firstAppearance[i]], blockMatrix[firstAppearance[j]], BLOCK_COLS * sizeof(float));
                memcpy(blockMatrix[firstAppearance[j]], temp, BLOCK_COLS * sizeof(float));

                /* Swap entries in firstAppearance */
                tempIndex = firstAppearance[i];
                firstAppearance[i] = firstAppearance[j];
                firstAppearance[j] = tempIndex;
            }
        }
    }

    /* Free temporary arrays */
    FREE(blockMatrix);
    FREE(sortedIndices);
    FREE(firstAppearance);

    return 1;
}

/**
 * @brief Create a new sequence with unique RF, gradient, and ADC events.
 * 
 * @param uniqueSeq Pointer to the output SeqFile structure for the unique sequence.
 * @param seq Pointer to the input sequence file.
 * @return int 1 if successful, 0 if failed.
 */
int getUniqueSeq(SeqFile* uniqueSeq, const SeqFile* seq) {
    int numUniqueBlocks;

    /* Generate unique sequence */
    if (!uniqueSequence(uniqueSeq, seq)) {
        return 0;
    }

    /* Generate unique block IDs directly in uniqueSeq */
    if (!getUniqueBlockIDs(uniqueSeq, seq, &numUniqueBlocks)) {
        return 0;
    }

    return 1;
}