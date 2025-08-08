/**
 * @file unique_sequence.c
 * @brief Implementation of functions for creating unique sequences.
 */

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "unique_sequence.h"
#include "vendor.h"
#include "seqfile.h"

/* Function declarations that must be visible to other modules */
int getUniqueSeq(SeqFile* uniqueSeq, const SeqFile* seq);
float getMaxRFAmplitude(const SeqFile* seq);
float getMaxGradientAmplitude(const SeqFile* seq);
float getMaxSlewRate(const SeqFile* seq);
int countNavigationADCEvents(const SeqFile* seq);

/* Internal function declarations */
static int uniqueSequence(SeqFile* uniqueSeq, const SeqFile* seq);

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
    const float* rfA = (const float*)a;
    const float* rfB = (const float*)b;
    
    /* Compare mag_id, phase_id, time_id, and delay */
    if (rfA[1] != rfB[1]) return rfA[1] > rfB[1] ? 1 : -1;  /* mag_id */
    if (rfA[2] != rfB[2]) return rfA[2] > rfB[2] ? 1 : -1;  /* phase_id */
    if (rfA[3] != rfB[3]) return rfA[3] > rfB[3] ? 1 : -1;  /* time_id */
    if (rfA[5] != rfB[5]) return rfA[5] > rfB[5] ? 1 : -1;  /* delay */
    
    return 0;  /* Events are identical */
}

/**
 * @brief Compare function for gradient events.
 */
static int compareGradEvents(const void* a, const void* b) {
    const float* gradA = (const float*)a;
    const float* gradB = (const float*)b;
    
    /* First check if types are different */
    if (gradA[0] != gradB[0]) return gradA[0] > gradB[0] ? 1 : -1;  /* type */
    
    if (gradA[0] == 0) {  /* Trapezoid */
        /* Compare rise, flat, fall, and delay */
        if (gradA[2] != gradB[2]) return gradA[2] > gradB[2] ? 1 : -1;  /* rise */
        if (gradA[3] != gradB[3]) return gradA[3] > gradB[3] ? 1 : -1;  /* flat */
        if (gradA[4] != gradB[4]) return gradA[4] > gradB[4] ? 1 : -1;  /* fall */
        if (gradA[6] != gradB[6]) return gradA[6] > gradB[6] ? 1 : -1;  /* delay */
    } else {  /* Arbitrary */
        /* Compare shape_id, first, last, time_id, and delay */
        if (gradA[4] != gradB[4]) return gradA[4] > gradB[4] ? 1 : -1;  /* shape_id */
        if (gradA[2] != gradB[2]) return gradA[2] > gradB[2] ? 1 : -1;  /* first */
        if (gradA[3] != gradB[3]) return gradA[3] > gradB[3] ? 1 : -1;  /* last */
        if (gradA[5] != gradB[5]) return gradA[5] > gradB[5] ? 1 : -1;  /* time_id */
        if (gradA[6] != gradB[6]) return gradA[6] > gradB[6] ? 1 : -1;  /* delay */
    }
    
    return 0;  /* Events are identical */
}

/**
 * @brief Compare function for ADC events.
 */
static int compareADCEvents(const void* a, const void* b) {
    const float* adcA = (const float*)a;
    const float* adcB = (const float*)b;
    
    /* Compare num, dwell, delay, and phase_id */
    if (adcA[0] != adcB[0]) return adcA[0] > adcB[0] ? 1 : -1;  /* num */
    if (adcA[1] != adcB[1]) return adcA[1] > adcB[1] ? 1 : -1;  /* dwell */
    if (adcA[2] != adcB[2]) return adcA[2] > adcB[2] ? 1 : -1;  /* delay */
    if (adcA[7] != adcB[7]) return adcA[7] > adcB[7] ? 1 : -1;  /* phase_id */
    
    return 0;  /* Events are identical */
}

/**
 * @brief Find unique RF events in the sequence.
 * 
 * @param seq Source sequence file
 * @param rfMap Output mapping from original indices to unique indices (1-based)
 * @param uniqueRfCount Output pointer to store number of unique RF events
 * @return float(*)[RF_COLS] Array of unique RF events, caller must free
 */
static float (*findUniqueRF(const SeqFile* seq, int* rfMap, int* uniqueRfCount))[RF_COLS] {
    int n = seq->rfLibrarySize;
    int i, j;
    int *sortedIndices = NULL;
    float (*rfMatrix)[3] = NULL;
    float (*uniqueRfLibrary)[RF_COLS] = NULL;
    
    *uniqueRfCount = 0;
    if (n == 0) return NULL;
    
    /* Allocate memory for sorted indices and RF comparison matrix */
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    rfMatrix = (float (*)[3])ALLOC(n * sizeof(float[3]));
    if (!sortedIndices || !rfMatrix) goto cleanup_error;
    
    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        rfMatrix[i][0] = seq->rfLibrary[i][1]; /* mag_id */
        rfMatrix[i][1] = seq->rfLibrary[i][2]; /* phase_id */
        rfMatrix[i][2] = seq->rfLibrary[i][3]; /* time_id */
        sortedIndices[i] = i;
    }
    
    /* Sort the indices based on the extracted values */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (compareRFEvents(rfMatrix[sortedIndices[i]], rfMatrix[sortedIndices[j]]) > 0) {
                int temp = sortedIndices[i];
                sortedIndices[i] = sortedIndices[j];
                sortedIndices[j] = temp;
            }
        }
    }
    
    /* Count unique events and create a mapping */
    *uniqueRfCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareRFEvents(rfMatrix[sortedIndices[i]], rfMatrix[sortedIndices[i-1]]) != 0) {
            (*uniqueRfCount)++;
        }
        /* Map from original indices to unique indices (1-based) */
        rfMap[sortedIndices[i]] = *uniqueRfCount;
    }
    
    /* Allocate memory for the unique library */
    uniqueRfLibrary = (float (*)[RF_COLS])ALLOC((*uniqueRfCount) * sizeof(float[RF_COLS]));
    if (!uniqueRfLibrary) goto cleanup_error;
    
    /* Build the unique library */
    *uniqueRfCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareRFEvents(rfMatrix[sortedIndices[i]], rfMatrix[sortedIndices[i-1]]) != 0) {
            /* Copy the entire row from the original library */
            memcpy(uniqueRfLibrary[*uniqueRfCount], seq->rfLibrary[sortedIndices[i]], RF_COLS * sizeof(float));
            (*uniqueRfCount)++;
        }
    }
    
    /* Free temporary memory */
    FREE(rfMatrix);
    FREE(sortedIndices);
    return uniqueRfLibrary;
    
cleanup_error:
    if (rfMatrix) FREE(rfMatrix);
    if (sortedIndices) FREE(sortedIndices);
    if (uniqueRfLibrary) FREE(uniqueRfLibrary);
    return NULL;
}

/**
 * @brief Find unique gradient events in the sequence.
 * 
 * @param seq Source sequence file
 * @param gradMap Output mapping from original indices to unique indices (1-based)
 * @param uniqueGradCount Output pointer to store number of unique gradient events
 * @return float(*)[GRAD_COLS] Array of unique gradient events, caller must free
 */
static float (*findUniqueGrad(const SeqFile* seq, int* gradMap, int* uniqueGradCount))[GRAD_COLS] {
    int n = seq->gradLibrarySize;
    int i, j, type;
    int *sortedIndices = NULL;
    float (*gradMatrix)[5] = NULL;
    float (*uniqueGradLibrary)[GRAD_COLS] = NULL;
    
    *uniqueGradCount = 0;
    if (n == 0) return NULL;
    
    /* Allocate memory for sorted indices and gradient comparison matrix */
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    gradMatrix = (float (*)[5])ALLOC(n * sizeof(float[5]));
    if (!sortedIndices || !gradMatrix) goto cleanup_error;
    
    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        type = (int)seq->gradLibrary[i][0];
        gradMatrix[i][0] = (float)type;
        
        if (type == 0) { /* TRAP */
            gradMatrix[i][1] = seq->gradLibrary[i][2]; /* rise */
            gradMatrix[i][2] = seq->gradLibrary[i][3]; /* flat */
            gradMatrix[i][3] = seq->gradLibrary[i][4]; /* fall */
            gradMatrix[i][4] = seq->gradLibrary[i][6]; /* delay */
        } else { /* ARB */
            gradMatrix[i][1] = seq->gradLibrary[i][2]; /* first */
            gradMatrix[i][2] = seq->gradLibrary[i][3]; /* last */
            gradMatrix[i][3] = seq->gradLibrary[i][4]; /* shape_id */
            gradMatrix[i][4] = seq->gradLibrary[i][6]; /* delay */
        }
        
        sortedIndices[i] = i;
    }
    
    /* Sort the indices based on the extracted values */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (compareGradEvents(gradMatrix[sortedIndices[i]], gradMatrix[sortedIndices[j]]) > 0) {
                int temp = sortedIndices[i];
                sortedIndices[i] = sortedIndices[j];
                sortedIndices[j] = temp;
            }
        }
    }
    
    /* Count unique events and create a mapping */
    *uniqueGradCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareGradEvents(gradMatrix[sortedIndices[i]], gradMatrix[sortedIndices[i-1]]) != 0) {
            (*uniqueGradCount)++;
        }
        /* Map from original indices to unique indices (1-based) */
        gradMap[sortedIndices[i]] = *uniqueGradCount;
    }
    
    /* Allocate memory for the unique library */
    uniqueGradLibrary = (float (*)[GRAD_COLS])ALLOC((*uniqueGradCount) * sizeof(float[GRAD_COLS]));
    if (!uniqueGradLibrary) goto cleanup_error;
    
    /* Build the unique library */
    *uniqueGradCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareGradEvents(gradMatrix[sortedIndices[i]], gradMatrix[sortedIndices[i-1]]) != 0) {
            /* Copy the entire row from the original library */
            memcpy(uniqueGradLibrary[*uniqueGradCount], seq->gradLibrary[sortedIndices[i]], GRAD_COLS * sizeof(float));
            (*uniqueGradCount)++;
        }
    }
    
    /* Free temporary memory */
    FREE(gradMatrix);
    FREE(sortedIndices);
    return uniqueGradLibrary;
    
cleanup_error:
    if (gradMatrix) FREE(gradMatrix);
    if (sortedIndices) FREE(sortedIndices);
    if (uniqueGradLibrary) FREE(uniqueGradLibrary);
    return NULL;
}

/**
 * @brief Find unique ADC events in the sequence.
 * 
 * @param seq Source sequence file
 * @param adcMap Output mapping from original indices to unique indices (1-based)
 * @param uniqueAdcCount Output pointer to store number of unique ADC events
 * @return float(*)[ADC_COLS] Array of unique ADC events, caller must free
 */
static float (*findUniqueADC(const SeqFile* seq, int* adcMap, int* uniqueAdcCount))[ADC_COLS] {
    int n = seq->adcLibrarySize;
    int i, j;
    int *sortedIndices = NULL;
    float (*adcMatrix)[3] = NULL;
    float (*uniqueAdcLibrary)[ADC_COLS] = NULL;
    
    *uniqueAdcCount = 0;
    if (n == 0) return NULL;
    
    /* Allocate memory for sorted indices and ADC comparison matrix */
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    adcMatrix = (float (*)[3])ALLOC(n * sizeof(float[3]));
    if (!sortedIndices || !adcMatrix) goto cleanup_error;
    
    /* Extract the identifying columns for comparison */
    for (i = 0; i < n; i++) {
        adcMatrix[i][0] = seq->adcLibrary[i][0]; /* num */
        adcMatrix[i][1] = seq->adcLibrary[i][1]; /* dwell */
        adcMatrix[i][2] = seq->adcLibrary[i][2]; /* delay */
        sortedIndices[i] = i;
    }
    
    /* Sort the indices based on the extracted values */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (compareADCEvents(adcMatrix[sortedIndices[i]], adcMatrix[sortedIndices[j]]) > 0) {
                int temp = sortedIndices[i];
                sortedIndices[i] = sortedIndices[j];
                sortedIndices[j] = temp;
            }
        }
    }
    
    /* Count unique events and create a mapping */
    *uniqueAdcCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareADCEvents(adcMatrix[sortedIndices[i]], adcMatrix[sortedIndices[i-1]]) != 0) {
            (*uniqueAdcCount)++;
        }
        /* Map from original indices to unique indices (1-based) */
        adcMap[sortedIndices[i]] = *uniqueAdcCount;
    }
    
    /* Allocate memory for the unique library */
    uniqueAdcLibrary = (float (*)[ADC_COLS])ALLOC((*uniqueAdcCount) * sizeof(float[ADC_COLS]));
    if (!uniqueAdcLibrary) goto cleanup_error;
    
    /* Build the unique library */
    *uniqueAdcCount = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareADCEvents(adcMatrix[sortedIndices[i]], adcMatrix[sortedIndices[i-1]]) != 0) {
            /* Copy the entire row from the original library */
            memcpy(uniqueAdcLibrary[*uniqueAdcCount], seq->adcLibrary[sortedIndices[i]], ADC_COLS * sizeof(float));
            (*uniqueAdcCount)++;
        }
    }
    
    /* Free temporary memory */
    FREE(adcMatrix);
    FREE(sortedIndices);
    return uniqueAdcLibrary;
    
cleanup_error:
    if (adcMatrix) FREE(adcMatrix);
    if (sortedIndices) FREE(sortedIndices);
    if (uniqueAdcLibrary) FREE(uniqueAdcLibrary);
    return NULL;
}

/**
 * @brief Create a new sequence with unique RF, gradient, and ADC events.
 * 
 * @param uniqueSeq Pointer to the output SeqFile structure for the unique sequence.
 * @param seq Pointer to the input sequence file.
 * @return int 1 if successful, 0 if failed.
 */
int uniqueSequence(SeqFile* uniqueSeq, const SeqFile* seq) {
    int i, j;
    int *rfMap = NULL, *gradXMap = NULL, *gradYMap = NULL, *gradZMap = NULL, *adcMap = NULL;
    int uniqueRfCount = 0, uniqueGradXCount = 0, uniqueGradYCount = 0, uniqueGradZCount = 0, uniqueAdcCount = 0;
    float (*uniqueRfLibrary)[RF_COLS] = NULL;
    float (*uniqueGradXLibrary)[GRAD_COLS] = NULL;
    float (*uniqueGradYLibrary)[GRAD_COLS] = NULL;
    float (*uniqueGradZLibrary)[GRAD_COLS] = NULL;
    float (*uniqueAdcLibrary)[ADC_COLS] = NULL;
    
    /* Initialize the output sequence */
    __seqFileReset(uniqueSeq);
    
    /* Copy basic information */
    uniqueSeq->filePath = strdup(seq->filePath);
    uniqueSeq->versionMajor = seq->versionMajor;
    uniqueSeq->versionMinor = seq->versionMinor;
    uniqueSeq->versionRevision = seq->versionRevision;
    uniqueSeq->versionCombined = seq->versionCombined;
    uniqueSeq->isVersionParsed = seq->isVersionParsed;
    
    /* Allocate temporary arrays to store the mapping between original and unique events */
    if (seq->rfLibrarySize > 0) {
        rfMap = (int*)ALLOC(seq->rfLibrarySize * sizeof(int));
        if (!rfMap) goto cleanup_error;
    }
    
    if (seq->gradLibrarySize > 0) {
        gradXMap = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
        gradYMap = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
        gradZMap = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
        if (!gradXMap || !gradYMap || !gradZMap) goto cleanup_error;
    }
    
    if (seq->adcLibrarySize > 0) {
        adcMap = (int*)ALLOC(seq->adcLibrarySize * sizeof(int));
        if (!adcMap) goto cleanup_error;
    }
    
    /* Find unique events for each library */
    
    /* Process RF library */
    if (seq->rfLibrarySize > 0) {
        uniqueRfLibrary = findUniqueRF(seq, rfMap, &uniqueRfCount);
        if (!uniqueRfLibrary) goto cleanup_error;
        
        uniqueSeq->rfLibrarySize = uniqueRfCount;
        uniqueSeq->rfLibrary = uniqueRfLibrary;
        uniqueSeq->isRfLibraryParsed = seq->isRfLibraryParsed;
    }
    
    /* Process gradient libraries - we process X, Y, Z separately as they may have different unique patterns */
    if (seq->gradLibrarySize > 0) {
        uniqueGradXLibrary = findUniqueGrad(seq, gradXMap, &uniqueGradXCount);
        uniqueGradYLibrary = findUniqueGrad(seq, gradYMap, &uniqueGradYCount);
        uniqueGradZLibrary = findUniqueGrad(seq, gradZMap, &uniqueGradZCount);
        
        if (!uniqueGradXLibrary || !uniqueGradYLibrary || !uniqueGradZLibrary) goto cleanup_error;
        
        /* For the unique sequence, we combine all unique gradient events */
        uniqueSeq->gradLibrarySize = uniqueGradXCount + uniqueGradYCount + uniqueGradZCount;
        uniqueSeq->gradLibrary = (float(*)[GRAD_COLS])ALLOC(uniqueSeq->gradLibrarySize * sizeof(float[GRAD_COLS]));
        if (!uniqueSeq->gradLibrary) goto cleanup_error;
        
        /* Copy unique gradient events to the combined library */
        for (i = 0; i < uniqueGradXCount; i++) {
            memcpy(uniqueSeq->gradLibrary[i], uniqueGradXLibrary[i], GRAD_COLS * sizeof(float));
        }
        
        for (i = 0; i < uniqueGradYCount; i++) {
            memcpy(uniqueSeq->gradLibrary[uniqueGradXCount + i], uniqueGradYLibrary[i], GRAD_COLS * sizeof(float));
        }
        
        for (i = 0; i < uniqueGradZCount; i++) {
            memcpy(uniqueSeq->gradLibrary[uniqueGradXCount + uniqueGradYCount + i], uniqueGradZLibrary[i], GRAD_COLS * sizeof(float));
        }
        
        /* Update the mappings to point to the combined library */
        for (i = 0; i < seq->gradLibrarySize; i++) {
            if (gradYMap[i] > 0) {
                gradYMap[i] += uniqueGradXCount;  /* Y events start after X events */
            }
            if (gradZMap[i] > 0) {
                gradZMap[i] += (uniqueGradXCount + uniqueGradYCount);  /* Z events start after X and Y events */
            }
        }
        
        uniqueSeq->isGradLibraryParsed = seq->isGradLibraryParsed;
        
        /* Free temporary gradient libraries */
        FREE(uniqueGradXLibrary);
        FREE(uniqueGradYLibrary);
        FREE(uniqueGradZLibrary);
        uniqueGradXLibrary = uniqueGradYLibrary = uniqueGradZLibrary = NULL;
    }
    
    /* Process ADC library */
    if (seq->adcLibrarySize > 0) {
        uniqueAdcLibrary = findUniqueADC(seq, adcMap, &uniqueAdcCount);
        if (!uniqueAdcLibrary) goto cleanup_error;
        
        uniqueSeq->adcLibrarySize = uniqueAdcCount;
        uniqueSeq->adcLibrary = uniqueAdcLibrary;
        uniqueSeq->isAdcLibraryParsed = seq->isAdcLibraryParsed;
    }
    
    /* Copy block library */
    if (seq->numBlocks > 0) {
        uniqueSeq->numBlocks = seq->numBlocks;
        uniqueSeq->blockLibrary = (float(*)[7])ALLOC(uniqueSeq->numBlocks * sizeof(float[7]));
        if (!uniqueSeq->blockLibrary) goto cleanup_error;
        
        /* Copy blocks but update the event IDs to use the unique events */
        for (i = 0; i < seq->numBlocks; i++) {
            uniqueSeq->blockLibrary[i][0] = seq->blockLibrary[i][0];  /* duration */
            
            /* Map RF event */
            j = (int)seq->blockLibrary[i][1];
            uniqueSeq->blockLibrary[i][1] = j > 0 ? rfMap[j - 1] : 0;
            
            /* Map gradient events */
            j = (int)seq->blockLibrary[i][2];
            uniqueSeq->blockLibrary[i][2] = j > 0 ? gradXMap[j - 1] : 0;
            
            j = (int)seq->blockLibrary[i][3];
            uniqueSeq->blockLibrary[i][3] = j > 0 ? gradYMap[j - 1] : 0;
            
            j = (int)seq->blockLibrary[i][4];
            uniqueSeq->blockLibrary[i][4] = j > 0 ? gradZMap[j - 1] : 0;
            
            /* Map ADC event */
            j = (int)seq->blockLibrary[i][5];
            uniqueSeq->blockLibrary[i][5] = j > 0 ? adcMap[j - 1] : 0;
            
            /* Extension ID */
            uniqueSeq->blockLibrary[i][6] = 0;  /* No extensions in the unique sequence */
        }
        
        uniqueSeq->isBlockLibraryParsed = seq->isBlockLibraryParsed;
    }
    
    /* Copy label limits from the original sequence */
    memcpy(&uniqueSeq->labelLimits, &seq->labelLimits, sizeof(LabelLimits));
    
    /* Free temporary mapping arrays */
    if (rfMap) FREE(rfMap);
    if (gradXMap) FREE(gradXMap);
    if (gradYMap) FREE(gradYMap);
    if (gradZMap) FREE(gradZMap);
    if (adcMap) FREE(adcMap);
    
    return 1;
    
cleanup_error:
    if (rfMap) FREE(rfMap);
    if (gradXMap) FREE(gradXMap);
    if (gradYMap) FREE(gradYMap);
    if (gradZMap) FREE(gradZMap);
    if (adcMap) FREE(adcMap);
    
    __seqFileFree(uniqueSeq);
    return 0;
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
int* getUniqueBlockIDs(const SeqFile* seq, int* numUniqueBlocks) {
    int n = seq->numBlocks;
    int i, j;
    float (*blockMatrix)[BLOCK_COLS] = NULL;
    int *sortedIndices = NULL;
    int *blockMap = NULL;
    
    *numUniqueBlocks = 0;
    if (n == 0) return NULL;
    
    /* Allocate memory for the block matrix and indices */
    blockMatrix = (float (*)[BLOCK_COLS])ALLOC(n * sizeof(float[BLOCK_COLS]));
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    blockMap = (int*)ALLOC(n * sizeof(int));
    
    if (!blockMatrix || !sortedIndices || !blockMap) goto cleanup_error;
    
    /* Copy block data to the matrix */
    for (i = 0; i < n; i++) {
        memcpy(blockMatrix[i], seq->blockLibrary[i], BLOCK_COLS * sizeof(float));
        sortedIndices[i] = i;
    }
    
    /* Sort the indices based on the matrix values */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (compareBlockRows(blockMatrix[sortedIndices[i]], blockMatrix[sortedIndices[j]]) > 0) {
                int temp = sortedIndices[i];
                sortedIndices[i] = sortedIndices[j];
                sortedIndices[j] = temp;
            }
        }
    }
    
    /* Find unique blocks and assign IDs (0-based) */
    *numUniqueBlocks = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compareBlockRows(blockMatrix[sortedIndices[i]], blockMatrix[sortedIndices[i-1]]) != 0) {
            (*numUniqueBlocks)++;
        }
        /* Map from original indices to unique indices (0-based) */
        blockMap[sortedIndices[i]] = (*numUniqueBlocks) - 1;
    }
    
    /* Clean up temporary arrays */
    FREE(blockMatrix);
    FREE(sortedIndices);
    
    return blockMap;
    
cleanup_error:
    if (blockMatrix) FREE(blockMatrix);
    if (sortedIndices) FREE(sortedIndices);
    if (blockMap) FREE(blockMap);
    return NULL;
}

/**
 * @brief Get maximum RF amplitude from a sequence file.
 * 
 * @param seq Pointer to the sequence file.
 * @return float The maximum RF amplitude found in the sequence.
 */
float getMaxRFAmplitude(const SeqFile* seq) {
    int i;
    float maxRFAmp = 0.0f;
    
    if (!seq || !seq->rfLibrary) {
        return 0.0f;
    }
    
    for (i = 0; i < seq->rfLibrarySize; i++) {
        /* RF amplitude is stored in the first column of the RF library */
        if (seq->rfLibrary[i][0] > maxRFAmp) {
            maxRFAmp = seq->rfLibrary[i][0];
        }
    }
    
    return maxRFAmp;
}

/**
 * @brief Get maximum gradient amplitude from a sequence file.
 * 
 * @param seq Pointer to the sequence file.
 * @return float The maximum gradient amplitude found in the sequence.
 */
float getMaxGradientAmplitude(const SeqFile* seq) {
    int i;
    float maxGradAmp = 0.0f;
    
    if (!seq || !seq->gradLibrary) {
        return 0.0f;
    }
    
    for (i = 0; i < seq->gradLibrarySize; i++) {
        /* Gradient amplitude is stored in the second column of the gradient library */
        if (fabs(seq->gradLibrary[i][1]) > maxGradAmp) {
            maxGradAmp = fabs(seq->gradLibrary[i][1]);
        }
    }
    
    return maxGradAmp;
}

/**
 * @brief Get maximum slew rate from a sequence file.
 * 
 * @param seq Pointer to the sequence file.
 * @return float The maximum slew rate found in the sequence.
 */
float getMaxSlewRate(const SeqFile* seq) {
    int i;
    float maxSlewRate = 0.0f;
    float slewRate;
    
    if (!seq || !seq->gradLibrary) {
        return 0.0f;
    }
    
    for (i = 0; i < seq->gradLibrarySize; i++) {
        if (seq->gradLibrary[i][0] == 0.0f) {  /* Trapezoid */
            /* Calculate slew rate as amp / rise time */
            if (seq->gradLibrary[i][2] > 0.0f) {  /* rise time */
                slewRate = fabs(seq->gradLibrary[i][1]) / seq->gradLibrary[i][2];
                if (slewRate > maxSlewRate) {
                    maxSlewRate = slewRate;
                }
            }
            
            /* Calculate slew rate as amp / fall time */
            if (seq->gradLibrary[i][4] > 0.0f) {  /* fall time */
                slewRate = fabs(seq->gradLibrary[i][1]) / seq->gradLibrary[i][4];
                if (slewRate > maxSlewRate) {
                    maxSlewRate = slewRate;
                }
            }
        } else {  /* Arbitrary - more complex calculation required */
            /* For arbitrary gradients, a proper implementation would need to
               analyze the shape data and calculate the maximum slew rate.
               This is left as a placeholder for now. */
        }
    }
    
    return maxSlewRate;
}

/**
 * @brief Create a new sequence with unique RF, gradient, and ADC events.
 * 
 * @param uniqueSeq Pointer to the output SeqFile structure for the unique sequence.
 * @param seq Pointer to the input sequence file.
 * @return int 1 if successful, 0 if failed.
 */
int getUniqueSeq(SeqFile* uniqueSeq, const SeqFile* seq) {
    /* Simply delegate to the existing uniqueSequence function */
    return uniqueSequence(uniqueSeq, seq);
}

/**
 * @brief Count the number of ADC events with the navigation flag.
 * 
 * @param seq Pointer to the sequence file.
 * @return int The number of ADC events with the navigation flag.
 */
int countNavigationADCEvents(const SeqFile* seq) {
    int i;
    int count = 0;
    int adcId, extId;
    int navFlagValue;
    
    if (!seq || !seq->blockLibrary) {
        return 0;
    }
    
    /* Scan all blocks to find ADC events with navigation flags */
    for (i = 0; i < seq->numBlocks; i++) {
        adcId = (int)seq->blockLibrary[i][5];
        extId = (int)seq->blockLibrary[i][6];
        
        /* Skip blocks without ADC events */
        if (adcId <= 0) continue;
        
        /* Check if this block has an extension that includes a navigation flag */
        if (extId > 0 && seq->extensionsLibrary && seq->extensionLUT) {
            int extType = (int)seq->extensionsLibrary[extId-1][0];
            
            /* Check if the extension type is a navigation flag (assuming type 1 = NAV) */
            if (extType == 1) {
                /* Get the navigation flag value */
                navFlagValue = (int)seq->extensionsLibrary[extId-1][1];
                
                /* If NAV flag is set to 1, increment count */
                if (navFlagValue == 1) {
                    count++;
                }
            }
        }
    }
    
    return count;
}
