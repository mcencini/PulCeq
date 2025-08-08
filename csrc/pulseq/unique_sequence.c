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
#include "pulseq/event.h" /* Include ShapeArbitrary type definition */

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
    if (matrixA[5] != matrixB[5]) return matrixA[5] < matrixB[5] ? -1 : 1;  /* delay */
    
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

    /* Compare type first as a defining characteristic */
    if (matrixA[0] != matrixB[0]) return matrixA[0] < matrixB[0] ? -1 : 1;  /* type */

    if (matrixA[0] == 0) {  /* Trapezoid */
        /* Compare rise, flat, fall, and delay */
        if (matrixA[1] != matrixB[1]) return matrixA[1] < matrixB[1] ? -1 : 1;  /* rise */
        if (matrixA[2] != matrixB[2]) return matrixA[2] < matrixB[2] ? -1 : 1;  /* flat */
        if (matrixA[3] != matrixB[3]) return matrixA[3] < matrixB[3] ? -1 : 1;  /* fall */
        if (matrixA[4] != matrixB[4]) return matrixA[4] < matrixB[4] ? -1 : 1;  /* delay */
    } else {  /* Arbitrary */
        /* Compare first, last, shape_id, time_id, and delay */
        if (matrixA[1] != matrixB[1]) return matrixA[1] < matrixB[1] ? -1 : 1;  /* first */
        if (matrixA[2] != matrixB[2]) return matrixA[2] < matrixB[2] ? -1 : 1;  /* last */
        if (matrixA[3] != matrixB[3]) return matrixA[3] < matrixB[3] ? -1 : 1;  /* shape_id */
        if (matrixA[4] != matrixB[4]) return matrixA[4] < matrixB[4] ? -1 : 1;  /* time_id */
        if (matrixA[5] != matrixB[5]) return matrixA[5] < matrixB[5] ? -1 : 1;  /* delay */
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
    if (matrixA[7] != matrixB[7]) return matrixA[7] < matrixB[7] ? -1 : 1;  /* phase_id */

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
    int i, uniqueIndex;
    float *rfMaxAmp;

    n = seq->rfLibrarySize;
    *uniqueRfCount = 0;
    if (n == 0) return;

    rfMaxAmp = (float*)ALLOC(n * sizeof(float));
    if (!rfMaxAmp) return;

    for (i = 0; i < n; i++) {
        rfMaxAmp[i] = 0.0f;
    }

    /* Allocate memory for sorted indices, RF comparison matrix, and first appearance tracking */
    int *sortedIndices = (int*)ALLOC(n * sizeof(int));
    float **rfMatrix = (float**)ALLOC(n * sizeof(float*));
    int *firstAppearance = (int*)ALLOC(n * sizeof(int)); /* Track first appearance of unique events */

    if (!sortedIndices || !rfMatrix || !firstAppearance) {
        if (sortedIndices) FREE(sortedIndices);
        if (rfMatrix) FREE(rfMatrix);
        if (firstAppearance) FREE(firstAppearance);
        FREE(rfMaxAmp);
        return;
    }

    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        rfMatrix[i] = (float*)ALLOC(3 * sizeof(float));
        if (!rfMatrix[i]) {
            for (int j = 0; j < i; j++) {
                FREE(rfMatrix[j]);
            }
            FREE(rfMatrix);
            FREE(sortedIndices);
            FREE(firstAppearance);
            FREE(rfMaxAmp);
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
        for (int j = i + 1; j < *uniqueRfCount; j++) {
            if (firstAppearance[i] > firstAppearance[j]) {
                /* Swap entries in uniqueRfLibrary */
                float temp[RF_COLS];
                memcpy(temp, uniqueRfLibrary[i], RF_COLS * sizeof(float));
                memcpy(uniqueRfLibrary[i], uniqueRfLibrary[j], RF_COLS * sizeof(float));
                memcpy(uniqueRfLibrary[j], temp, RF_COLS * sizeof(float));

                /* Swap entries in firstAppearance */
                int tempIndex = firstAppearance[i];
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

    for (i = 0; i < n; i++) {
        uniqueIndex = rfMap[i] - 1;
        if (seq->rfLibrary[i][0] > rfMaxAmp[uniqueIndex]) {
            rfMaxAmp[uniqueIndex] = seq->rfLibrary[i][0];
        }
    }

    for (i = 0; i < *uniqueRfCount; i++) {
        uniqueRfLibrary[i][0] = rfMaxAmp[i];
    }

    /* Free temporary memory */
    for (i = 0; i < n; i++) {
        FREE(rfMatrix[i]);
    }
    FREE(rfMatrix);
    FREE(sortedIndices);
    FREE(firstAppearance);
    FREE(rfMaxAmp);
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
    int i, uniqueIndex;
    float *gradMaxAmp;

    n = seq->gradLibrarySize;
    *uniqueGradCount = 0;
    if (n == 0) return;

    gradMaxAmp = (float*)ALLOC(n * sizeof(float));
    if (!gradMaxAmp) return;

    for (i = 0; i < n; i++) {
        gradMaxAmp[i] = 0.0f;
    }

    /* Allocate memory for sorted indices, gradient comparison matrix, and first appearance tracking */
    int *sortedIndices = (int*)ALLOC(n * sizeof(int));
    float **gradMatrix = (float**)ALLOC(n * sizeof(float*));
    int *firstAppearance = (int*)ALLOC(n * sizeof(int)); /* Track first appearance of unique events */

    if (!sortedIndices || !gradMatrix || !firstAppearance) {
        if (sortedIndices) FREE(sortedIndices);
        if (gradMatrix) FREE(gradMatrix);
        if (firstAppearance) FREE(firstAppearance);
        FREE(gradMaxAmp);
        return;
    }

    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        gradMatrix[i] = (float*)ALLOC(5 * sizeof(float));
        if (!gradMatrix[i]) {
            for (int j = 0; j < i; j++) {
                FREE(gradMatrix[j]);
            }
            FREE(gradMatrix);
            FREE(sortedIndices);
            FREE(firstAppearance);
            FREE(gradMaxAmp);
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
        for (int j = i + 1; j < *uniqueGradCount; j++) {
            if (firstAppearance[i] > firstAppearance[j]) {
                /* Swap entries in uniqueGradLibrary */
                float temp[GRAD_COLS];
                memcpy(temp, uniqueGradLibrary[i], GRAD_COLS * sizeof(float));
                memcpy(uniqueGradLibrary[i], uniqueGradLibrary[j], GRAD_COLS * sizeof(float));
                memcpy(uniqueGradLibrary[j], temp, GRAD_COLS * sizeof(float));

                /* Swap entries in firstAppearance */
                int tempIndex = firstAppearance[i];
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

    for (i = 0; i < n; i++) {
        uniqueIndex = gradMap[i] - 1;
        if (seq->gradLibrary[i][0] > gradMaxAmp[uniqueIndex]) {
            gradMaxAmp[uniqueIndex] = seq->gradLibrary[i][0];
        }
    }

    for (i = 0; i < *uniqueGradCount; i++) {
        uniqueGradLibrary[i][0] = gradMaxAmp[i];
    }

    /* Free temporary memory */
    for (i = 0; i < n; i++) {
        FREE(gradMatrix[i]);
    }
    FREE(gradMatrix);
    FREE(sortedIndices);
    FREE(firstAppearance);
    FREE(gradMaxAmp);
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
    int i;
    int uniqueRfCount, uniqueGradCount, uniqueAdcCount;
    int *rfMap, *gradMap, *adcMap;
    float (*uniqueRfLibrary)[RF_COLS];
    float (*uniqueGradLibrary)[GRAD_COLS];
    float (*uniqueAdcLibrary)[ADC_COLS];

    /* Initialize uniqueSeq */
    __seqFileReset(uniqueSeq);

    /* Allocate memory for mapping arrays */
    rfMap = (int*)ALLOC(seq->rfLibrarySize * sizeof(int));
    gradMap = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
    adcMap = (int*)ALLOC(seq->adcLibrarySize * sizeof(int));

    if (!rfMap || !gradMap || !adcMap) {
        if (rfMap) FREE(rfMap);
        if (gradMap) FREE(gradMap);
        if (adcMap) FREE(adcMap);
        return 0;
    }

    /* Deduplicate RF events */
    uniqueRfLibrary = (float (*)[RF_COLS])ALLOC(seq->rfLibrarySize * sizeof(float[RF_COLS]));
    if (!uniqueRfLibrary) {
        FREE(rfMap);
        FREE(gradMap);
        FREE(adcMap);
        return 0;
    }
    findUniqueRF(seq, rfMap, &uniqueRfCount, uniqueRfLibrary);
    uniqueSeq->rfLibrary = uniqueRfLibrary;
    uniqueSeq->rfLibrarySize = uniqueRfCount;
    uniqueSeq->isRfLibraryParsed = 1;

    /* Deduplicate gradient events */
    uniqueGradLibrary = (float (*)[GRAD_COLS])ALLOC(seq->gradLibrarySize * sizeof(float[GRAD_COLS]));
    if (!uniqueGradLibrary) {
        FREE(rfMap);
        FREE(gradMap);
        FREE(adcMap);
        FREE(uniqueRfLibrary);
        return 0;
    }
    findUniqueGrad(seq, gradMap, &uniqueGradCount, uniqueGradLibrary);
    uniqueSeq->gradLibrary = uniqueGradLibrary;
    uniqueSeq->gradLibrarySize = uniqueGradCount;
    uniqueSeq->isGradLibraryParsed = 1;

    /* Deduplicate ADC events */
    uniqueAdcLibrary = (float (*)[ADC_COLS])ALLOC(seq->adcLibrarySize * sizeof(float[ADC_COLS]));
    if (!uniqueAdcLibrary) {
        FREE(rfMap);
        FREE(gradMap);
        FREE(adcMap);
        FREE(uniqueRfLibrary);
        FREE(uniqueGradLibrary);
        return 0;
    }
    findUniqueADC(seq, adcMap, &uniqueAdcCount, uniqueAdcLibrary);
    uniqueSeq->adcLibrary = uniqueAdcLibrary;
    uniqueSeq->adcLibrarySize = uniqueAdcCount;
    uniqueSeq->isAdcLibraryParsed = 1;

    /* Copy shape library */
    uniqueSeq->shapesLibrarySize = seq->shapesLibrarySize;
    uniqueSeq->shapesLibrary = (ShapeArbitrary*)ALLOC(seq->shapesLibrarySize * sizeof(ShapeArbitrary));
    if (!uniqueSeq->shapesLibrary) {
        FREE(rfMap);
        FREE(gradMap);
        FREE(adcMap);
        FREE(uniqueRfLibrary);
        FREE(uniqueGradLibrary);
        FREE(uniqueAdcLibrary);
        return 0;
    }
    for (i = 0; i < seq->shapesLibrarySize; i++) {
        uniqueSeq->shapesLibrary[i] = seq->shapesLibrary[i];
    }

    /* Rebuild block library */
    uniqueSeq->numBlocks = seq->numBlocks;
    uniqueSeq->blockLibrary = (float(*)[BLOCK_COLS])ALLOC(seq->numBlocks * sizeof(float[BLOCK_COLS]));
    if (!uniqueSeq->blockLibrary) {
        FREE(rfMap);
        FREE(gradMap);
        FREE(adcMap);
        FREE(uniqueRfLibrary);
        FREE(uniqueGradLibrary);
        FREE(uniqueAdcLibrary);
        FREE(uniqueSeq->shapesLibrary);
        return 0;
    }
    for (i = 0; i < seq->numBlocks; i++) {
        uniqueSeq->blockLibrary[i][0] = seq->blockLibrary[i][0]; /* Copy duration */
        uniqueSeq->blockLibrary[i][1] = rfMap[(int)seq->blockLibrary[i][1] - 1];
        uniqueSeq->blockLibrary[i][2] = gradMap[(int)seq->blockLibrary[i][2] - 1];
        uniqueSeq->blockLibrary[i][3] = gradMap[(int)seq->blockLibrary[i][3] - 1];
        uniqueSeq->blockLibrary[i][4] = gradMap[(int)seq->blockLibrary[i][4] - 1];
        uniqueSeq->blockLibrary[i][5] = adcMap[(int)seq->blockLibrary[i][5] - 1];
        uniqueSeq->blockLibrary[i][6] = 0; /* Set extension ID to 0 */
    }
    uniqueSeq->isBlockLibraryParsed = 1;

    /* Free temporary arrays */
    FREE(rfMap);
    FREE(gradMap);
    FREE(adcMap);

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