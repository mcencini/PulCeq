/**
 * @file block.c
 * @brief Implements unique event and block identification for Pulseq sequence files.
 */

#include <string.h>

#include "../../pulseq.h"
#include "../../pulseq/alloc.h"

#include "block.h"

/**
 * @brief Comparison function for RF rows.
 * 
 * @param[in] a Pointer to first RF row.
 * @param[in] b Pointer to second RF row.
 * @return -1 if a < b, 1 if a > b, 0 if equal.
 */
int compare_rf_rows(const void* a, const void* b)
{
    const float* fa = (const float*)a;
    const float* fb = (const float*)b;
    int i;
    
    /* Compare mag_id, phase_id, time_id */
    for (i = 0; i < 3; i++) {
        if (fa[i] < fb[i]) return -1;
        if (fa[i] > fb[i]) return 1;
    }
    return 0;
}

/**
 * @brief Comparison function for gradient rows.
 * 
 * @param[in] a Pointer to first gradient row.
 * @param[in] b Pointer to second gradient row.
 * @return -1 if a < b, 1 if a > b, 0 if equal.
 */
int compare_grad_rows(const void* a, const void* b)
{
    const float* fa = (const float*)a;
    const float* fb = (const float*)b;
    int type_a, type_b;
    
    /* First compare type */
    type_a = (int)fa[0];
    type_b = (int)fb[0];
    
    if (type_a < type_b) return -1;
    if (type_a > type_b) return 1;
    
    /* If both are TRAP */
    if (type_a == 0) {
        /* Compare rise, flat, fall, delay */
        if (fa[1] < fb[1]) return -1;
        if (fa[1] > fb[1]) return 1;
        if (fa[2] < fb[2]) return -1;
        if (fa[2] > fb[2]) return 1;
        if (fa[3] < fb[3]) return -1;
        if (fa[3] > fb[3]) return 1;
        if (fa[4] < fb[4]) return -1;
        if (fa[4] > fb[4]) return 1;
    }
    /* If both are ARB/GRAD */
    else {
        /* Compare first, last, shape_id, delay */
        if (fa[1] < fb[1]) return -1;
        if (fa[1] > fb[1]) return 1;
        if (fa[2] < fb[2]) return -1;
        if (fa[2] > fb[2]) return 1;
        if (fa[3] < fb[3]) return -1;
        if (fa[3] > fb[3]) return 1;
        if (fa[4] < fb[4]) return -1;
        if (fa[4] > fb[4]) return 1;
    }
    
    return 0;
}

/**
 * @brief Comparison function for ADC rows.
 * 
 * @param[in] a Pointer to first ADC row.
 * @param[in] b Pointer to second ADC row.
 * @return -1 if a < b, 1 if a > b, 0 if equal.
 */
int compare_adc_rows(const void* a, const void* b)
{
    const float* fa = (const float*)a;
    const float* fb = (const float*)b;
    
    /* Compare numSamples, dwell, delay */
    if (fa[0] < fb[0]) return -1;
    if (fa[0] > fb[0]) return 1;
    if (fa[1] < fb[1]) return -1;
    if (fa[1] > fb[1]) return 1;
    if (fa[2] < fb[2]) return -1;
    if (fa[2] > fb[2]) return 1;
    
    return 0;
}

/**
 * @brief Comparison function for block rows.
 * 
 * @param[in] a Pointer to first block row.
 * @param[in] b Pointer to second block row.
 * @return -1 if a < b, 1 if a > b, 0 if equal.
 */
int compare_block_rows(const void* a, const void* b)
{
    const float* fa = (const float*)a;
    const float* fb = (const float*)b;
    int i;
    
    for (i = 0; i < BLOCK_COMPARE_COLS; i++) {
        if (fa[i] < fb[i]) return -1;
        if (fa[i] > fb[i]) return 1;
    }
    return 0;
}

/**
 * @brief Finds unique RF events in a Pulseq sequence file.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @param[out] unique_rfID Mapping from original RF IDs to unique IDs.
 * @param[out] unique_rfLibrary Array of unique RF events.
 * @return Number of unique RF events found.
 */
int findUniqueRF(const pulseq_SeqFile* seq, int* unique_rfID, float** unique_rfLibrary)
{
    int n, i, j, numUnique, *sortedIndices;
    float (*rfMatrix)[3];
    
    n = seq->rfLibrarySize;
    if (n == 0) return 0;
    
    /* Allocate memory for the RF matrix and indices */
    rfMatrix = (float (*)[3])ALLOC(n * sizeof(float[3]));
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    
    /* Build matrix of defining columns (mag_id, phase_id, time_id) */
    for (i = 0; i < n; i++) {
        rfMatrix[i][0] = seq->rfLibrary[i][1]; /* mag_id */
        rfMatrix[i][1] = seq->rfLibrary[i][2]; /* phase_id */
        rfMatrix[i][2] = seq->rfLibrary[i][3]; /* time_id */
        sortedIndices[i] = i;                  /* Initial order */
    }
    
    /* Sort the indices based on the matrix values */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (compare_rf_rows(rfMatrix[sortedIndices[i]], rfMatrix[sortedIndices[j]]) > 0) {
                int temp = sortedIndices[i];
                sortedIndices[i] = sortedIndices[j];
                sortedIndices[j] = temp;
            }
        }
    }
    
    /* Find unique rows and assign IDs (starting from 1) */
    numUnique = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compare_rf_rows(rfMatrix[sortedIndices[i]], rfMatrix[sortedIndices[i-1]]) != 0) {
            /* Copy the full RF event to unique library */
            unique_rfLibrary[numUnique] = (float*)ALLOC(RF_LIBSIZE * sizeof(float));
            memcpy(unique_rfLibrary[numUnique], seq->rfLibrary[sortedIndices[i]], RF_LIBSIZE * sizeof(float));
            numUnique++;
        }
        
        /* Map the original ID to the unique ID (IDs are 1-based, indices 0-based) */
        unique_rfID[sortedIndices[i]] = numUnique;
    }
    
    FREE(rfMatrix);
    FREE(sortedIndices);
    
    return numUnique;
}

/**
 * @brief Finds unique gradient events in a Pulseq sequence file.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @param[out] unique_gradID Mapping from original gradient IDs to unique IDs.
 * @param[out] unique_gradLibrary Array of unique gradient events.
 * @return Number of unique gradient events found.
 */
int findUniqueGrad(const pulseq_SeqFile* seq, int* unique_gradID, float** unique_gradLibrary)
{
    int n, i, j, numUnique, *sortedIndices, type;
    float (*gradMatrix)[5];
    
    n = seq->gradLibrarySize;
    if (n == 0) return 0;
    
    /* Allocate memory for the gradient matrix and indices */
    gradMatrix = (float (*)[5])ALLOC(n * sizeof(float[5]));
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    
    /* Build matrix of defining columns */
    for (i = 0; i < n; i++) {
        type = (int)seq->gradLibrary[i][0];
        gradMatrix[i][0] = (float)type;
        
        if (type == 0) { /* TRAP */
            gradMatrix[i][1] = seq->gradLibrary[i][2]; /* rise */
            gradMatrix[i][2] = seq->gradLibrary[i][3]; /* flat */
            gradMatrix[i][3] = seq->gradLibrary[i][4]; /* fall */
            gradMatrix[i][4] = seq->gradLibrary[i][5]; /* delay */
        } else { /* ARB/GRAD */
            gradMatrix[i][1] = seq->gradLibrary[i][1]; /* first */
            gradMatrix[i][2] = seq->gradLibrary[i][2]; /* last */
            gradMatrix[i][3] = seq->gradLibrary[i][4]; /* shape_id */
            gradMatrix[i][4] = seq->gradLibrary[i][6]; /* delay */
        }
        
        sortedIndices[i] = i; /* Initial order */
    }
    
    /* Sort the indices based on the matrix values */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (compare_grad_rows(gradMatrix[sortedIndices[i]], gradMatrix[sortedIndices[j]]) > 0) {
                int temp = sortedIndices[i];
                sortedIndices[i] = sortedIndices[j];
                sortedIndices[j] = temp;
            }
        }
    }
    
    /* Find unique rows and assign IDs (starting from 1) */
    numUnique = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compare_grad_rows(gradMatrix[sortedIndices[i]], gradMatrix[sortedIndices[i-1]]) != 0) {
            /* Copy the full gradient event to unique library */
            unique_gradLibrary[numUnique] = (float*)ALLOC(GRAD_LIBSIZE * sizeof(float));
            memcpy(unique_gradLibrary[numUnique], seq->gradLibrary[sortedIndices[i]], GRAD_LIBSIZE * sizeof(float));
            numUnique++;
        }
        
        /* Map the original ID to the unique ID (IDs are 1-based, indices 0-based) */
        unique_gradID[sortedIndices[i]] = numUnique;
    }
    
    FREE(gradMatrix);
    FREE(sortedIndices);
    
    return numUnique;
}

/**
 * @brief Finds unique ADC events in a Pulseq sequence file.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @param[out] unique_adcID Mapping from original ADC IDs to unique IDs.
 * @param[out] unique_adcLibrary Array of unique ADC events.
 * @return Number of unique ADC events found.
 */
int findUniqueADC(const pulseq_SeqFile* seq, int* unique_adcID, float** unique_adcLibrary)
{
    int n, i, j, numUnique, *sortedIndices;
    float (*adcMatrix)[3];
    
    n = seq->adcLibrarySize;
    if (n == 0) return 0;
    
    /* Allocate memory for the ADC matrix and indices */
    adcMatrix = (float (*)[3])ALLOC(n * sizeof(float[3]));
    sortedIndices = (int*)ALLOC(n * sizeof(int));
    
    /* Build matrix of defining columns (numSamples, dwell, delay) */
    for (i = 0; i < n; i++) {
        adcMatrix[i][0] = seq->adcLibrary[i][0]; /* numSamples */
        adcMatrix[i][1] = seq->adcLibrary[i][1]; /* dwell */
        adcMatrix[i][2] = seq->adcLibrary[i][2]; /* delay */
        sortedIndices[i] = i;                    /* Initial order */
    }
    
    /* Sort the indices based on the matrix values */
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (compare_adc_rows(adcMatrix[sortedIndices[i]], adcMatrix[sortedIndices[j]]) > 0) {
                int temp = sortedIndices[i];
                sortedIndices[i] = sortedIndices[j];
                sortedIndices[j] = temp;
            }
        }
    }
    
    /* Find unique rows and assign IDs (starting from 1) */
    numUnique = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compare_adc_rows(adcMatrix[sortedIndices[i]], adcMatrix[sortedIndices[i-1]]) != 0) {
            /* Copy the full ADC event to unique library */
            unique_adcLibrary[numUnique] = (float*)ALLOC(ADC_LIBSIZE * sizeof(float));
            memcpy(unique_adcLibrary[numUnique], seq->adcLibrary[sortedIndices[i]], ADC_LIBSIZE * sizeof(float));
            numUnique++;
        }
        
        /* Map the original ID to the unique ID (IDs are 1-based, indices 0-based) */
        unique_adcID[sortedIndices[i]] = numUnique;
    }
    
    FREE(adcMatrix);
    FREE(sortedIndices);
    
    return numUnique;
}

/**
 * @brief Public interface for unique ADC events.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @param[out] result Pointer to structure to store unique ADC results.
 */
void getUniqueADC(const pulseq_SeqFile* seq, UniqueADCResult* result)
{
    int i, numUniqueADC;
    float** unique_adcLibrary;
    int* unique_adcID;
    
    /* Allocate memory for the mapping and library */
    unique_adcID = (int*)ALLOC(seq->adcLibrarySize * sizeof(int));
    unique_adcLibrary = (float**)ALLOC(seq->adcLibrarySize * sizeof(float*));
    
    /* Find unique ADC events */
    numUniqueADC = findUniqueADC(seq, unique_adcID, unique_adcLibrary);
    
    /* Set the result */
    result->unique_adcID = unique_adcID;
    result->unique_adcLibrary = unique_adcLibrary;
    result->numUniqueADC = numUniqueADC;
}

/**
 * @brief Creates a new sequence with unique events.
 * 
 * @param[in] seq Original sequence file.
 * @param[out] unique_rfID Mapping from original RF IDs to unique IDs.
 * @param[out] unique_gradID Mapping from original gradient IDs to unique IDs.
 * @param[out] unique_adcID Mapping from original ADC IDs to unique IDs.
 * @return New sequence file with unique events.
 */
pulseq_SeqFile* createUniqueSeq(const pulseq_SeqFile* seq, int* unique_rfID, int* unique_gradID, int* unique_adcID)
{
    pulseq_SeqFile* unique_seq;
    float** unique_rfLibrary;
    float** unique_gradLibrary;
    float** unique_adcLibrary;
    int numUniqueRF, numUniqueGrad, numUniqueADC, i;
    
    /* Create a new sequence file */
    unique_seq = pulseq_createSeqFile();
    
    /* Allocate memory for the libraries */
    unique_rfLibrary = (float**)ALLOC(seq->rfLibrarySize * sizeof(float*));
    unique_gradLibrary = (float**)ALLOC(seq->gradLibrarySize * sizeof(float*));
    unique_adcLibrary = (float**)ALLOC(seq->adcLibrarySize * sizeof(float*));
    
    /* Find unique events for each library */
    numUniqueRF = findUniqueRF(seq, unique_rfID, unique_rfLibrary);
    numUniqueGrad = findUniqueGrad(seq, unique_gradID, unique_gradLibrary);
    numUniqueADC = findUniqueADC(seq, unique_adcID, unique_adcLibrary);
    
    /* Set up unique_seq with the unique libraries */
    unique_seq->rfLibrarySize = numUniqueRF;
    unique_seq->rfLibrary = (float**)ALLOC(numUniqueRF * sizeof(float*));
    for (i = 0; i < numUniqueRF; i++) {
        unique_seq->rfLibrary[i] = unique_rfLibrary[i];
    }
    
    unique_seq->gradLibrarySize = numUniqueGrad;
    unique_seq->gradLibrary = (float**)ALLOC(numUniqueGrad * sizeof(float*));
    for (i = 0; i < numUniqueGrad; i++) {
        unique_seq->gradLibrary[i] = unique_gradLibrary[i];
    }
    
    unique_seq->adcLibrarySize = numUniqueADC;
    unique_seq->adcLibrary = (float**)ALLOC(numUniqueADC * sizeof(float*));
    for (i = 0; i < numUniqueADC; i++) {
        unique_seq->adcLibrary[i] = unique_adcLibrary[i];
    }
    
    FREE(unique_rfLibrary);
    FREE(unique_gradLibrary);
    FREE(unique_adcLibrary);
    
    return unique_seq;
}

/**
 * @brief Gets unique blocks and sequence with only unique events.
 * 
 * @param[in] seq Original sequence file.
 * @return UniqueSeqResult containing unique blocks and sequence.
 */
UniqueSeqResult* getUniqueSeq(const pulseq_SeqFile* seq)
{
    int n, i, j, blockIdx, numUniqueBlocks;
    int *unique_rfID, *unique_gradID, *unique_adcID;
    pulseq_SeqFile* unique_seq;
    float (*blockMatrix)[BLOCK_COMPARE_COLS];
    int* blockToUnique;
    BlockDefinition* uniqueBlocks;
    UniqueSeqResult* result;
    
    n = seq->numBlocks;
    
    /* Allocate memory for mappings */
    unique_rfID = (int*)ALLOC(seq->rfLibrarySize * sizeof(int));
    unique_gradID = (int*)ALLOC(seq->gradLibrarySize * sizeof(int));
    unique_adcID = (int*)ALLOC(seq->adcLibrarySize * sizeof(int));
    
    /* Create sequence with unique events */
    unique_seq = createUniqueSeq(seq, unique_rfID, unique_gradID, unique_adcID);
    
    /* Create block matrix with unique event references */
    blockMatrix = (float (*)[BLOCK_COMPARE_COLS])ALLOC(n * sizeof(float[BLOCK_COMPARE_COLS]));
    for (i = 0; i < n; i++) {
        int col = 0;
        int rf_idx, gx_idx, gy_idx, gz_idx, adc_idx;
        
        /* duration */
        blockMatrix[i][col++] = seq->blockLibrary[i][0];
        
        /* Map to unique event IDs */
        rf_idx = (int)seq->blockLibrary[i][1] - 1;
        gx_idx = (int)seq->blockLibrary[i][2] - 1;
        gy_idx = (int)seq->blockLibrary[i][3] - 1;
        gz_idx = (int)seq->blockLibrary[i][4] - 1;
        adc_idx = (int)seq->blockLibrary[i][5] - 1;
        
        blockMatrix[i][col++] = (rf_idx >= 0) ? unique_rfID[rf_idx] : 0;
        blockMatrix[i][col++] = (gx_idx >= 0) ? unique_gradID[gx_idx] : 0;
        blockMatrix[i][col++] = (gy_idx >= 0) ? unique_gradID[gy_idx] : 0;
        blockMatrix[i][col++] = (gz_idx >= 0) ? unique_gradID[gz_idx] : 0;
        blockMatrix[i][col++] = (adc_idx >= 0) ? unique_adcID[adc_idx] : 0;
        
        /* Trigger info is kept the same as in original */
        blockMatrix[i][col++] = seq->blockLibrary[i][6];
    }
    
    /* Sort the block matrix */
    qsort(blockMatrix, n, sizeof(float[BLOCK_COMPARE_COLS]), compare_block_rows);
    
    /* Find unique blocks */
    blockToUnique = (int*)ALLOC(n * sizeof(int));
    uniqueBlocks = (BlockDefinition*)ALLOC(n * sizeof(BlockDefinition));
    
    numUniqueBlocks = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compare_block_rows(blockMatrix[i], blockMatrix[i-1]) != 0) {
            uniqueBlocks[numUniqueBlocks].duration = (int)blockMatrix[i][0];
            uniqueBlocks[numUniqueBlocks].rfID = (int)blockMatrix[i][1];
            uniqueBlocks[numUniqueBlocks].gxID = (int)blockMatrix[i][2];
            uniqueBlocks[numUniqueBlocks].gyID = (int)blockMatrix[i][3];
            uniqueBlocks[numUniqueBlocks].gzID = (int)blockMatrix[i][4];
            uniqueBlocks[numUniqueBlocks].adcID = (int)blockMatrix[i][5];
            numUniqueBlocks++;
        }
        
        /* Map from original blocks to unique blocks */
        blockToUnique[i] = numUniqueBlocks - 1;
    }
    
    /* Set up block library in unique_seq */
    unique_seq->numBlocks = numUniqueBlocks;
    unique_seq->blockLibrary = (float**)ALLOC(numUniqueBlocks * sizeof(float*));
    for (i = 0; i < numUniqueBlocks; i++) {
        unique_seq->blockLibrary[i] = (float*)ALLOC(BLOCK_LIBSIZE * sizeof(float));
        unique_seq->blockLibrary[i][0] = (float)uniqueBlocks[i].duration;
        unique_seq->blockLibrary[i][1] = (float)uniqueBlocks[i].rfID;
        unique_seq->blockLibrary[i][2] = (float)uniqueBlocks[i].gxID;
        unique_seq->blockLibrary[i][3] = (float)uniqueBlocks[i].gyID;
        unique_seq->blockLibrary[i][4] = (float)uniqueBlocks[i].gzID;
        unique_seq->blockLibrary[i][5] = (float)uniqueBlocks[i].adcID;
    }
    
    /* Create result structure */
    result = (UniqueSeqResult*)ALLOC(sizeof(UniqueSeqResult));
    result->unique_seq = unique_seq;
    result->uniqueBlocks = uniqueBlocks;
    result->numUniqueBlocks = numUniqueBlocks;
    result->blockToUnique = blockToUnique;
    result->unique_rfID = unique_rfID;
    result->unique_gradID = unique_gradID;
    result->unique_adcID = unique_adcID;
    
    FREE(blockMatrix);
    
    return result;
}

/**
 * @brief For backward compatibility - finds unique blocks in a Pulseq sequence file.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @return Pointer to UniqueBlocksResult containing unique blocks and mapping.
 */
UniqueBlocksResult* getUniqueBlocks(const pulseq_SeqFile* seq)
{
    UniqueSeqResult* uniqueSeqResult;
    UniqueBlocksResult* result;
    
    /* Use the new implementation and extract just the block information */
    uniqueSeqResult = getUniqueSeq(seq);
    
    /* Create legacy result structure */
    result = (UniqueBlocksResult*)ALLOC(sizeof(UniqueBlocksResult));
    result->uniqueBlocks = uniqueSeqResult->uniqueBlocks;
    result->numUniqueBlocks = uniqueSeqResult->numUniqueBlocks;
    result->blockToUnique = uniqueSeqResult->blockToUnique;
    
    /* Free the UniqueSeqResult but keep the blocks */
    uniqueSeqResult->uniqueBlocks = NULL;
    uniqueSeqResult->blockToUnique = NULL;
    freeUniqueSeqResult(uniqueSeqResult);
    
    return result;
}

/**
 * @brief Frees memory allocated for a UniqueBlocksResult structure.
 *
 * @param[in] result Pointer to UniqueBlocksResult to free.
 */
void freeUniqueBlocksResult(UniqueBlocksResult* result)
{
    if (!result) return;
    if (result->uniqueBlocks) FREE(result->uniqueBlocks);
    if (result->blockToUnique) FREE(result->blockToUnique);
    FREE(result);
}

/**
 * @brief Frees memory allocated for a UniqueSeqResult structure.
 *
 * @param[in] result Pointer to UniqueSeqResult to free.
 */
void freeUniqueSeqResult(UniqueSeqResult* result)
{
    if (!result) return;
    if (result->uniqueBlocks) FREE(result->uniqueBlocks);
    if (result->blockToUnique) FREE(result->blockToUnique);
    if (result->unique_rfID) FREE(result->unique_rfID);
    if (result->unique_gradID) FREE(result->unique_gradID);
    if (result->unique_adcID) FREE(result->unique_adcID);
    if (result->unique_seq) pulseq_freeSeqFile(result->unique_seq);
    FREE(result);
}