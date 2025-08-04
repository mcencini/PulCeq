/**
 * @file block.c
 * @brief Implements unique block identification for Pulseq sequence files.
 */

#include <string.h>

#include "../../pulseq.h"
#include "../../pulseq/alloc.h"

#include "block.h"

/**
 * @brief Comparison function for block rows.
 * 
 * Used for sorting and identifying unique blocks.
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
 * @brief Finds unique blocks in a Pulseq sequence file.
 * 
 * @param[in] seq Pointer to Pulseq sequence file.
 * @return Pointer to UniqueBlocksResult containing unique blocks and mapping.
 */
UniqueBlocksResult* getUniqueBlocks(const pulseq_SeqFile* seq)
{
    int n, i, j, axis, grad_idx, rf_idx, adc_idx, ext_idx, col, type, ref, found, numUnique;
    float* extData;
    float* trigData;
    float* gradData;
    float* rfData;
    float* adcData;
    float* blockRow;
    float (*matrix)[BLOCK_COMPARE_COLS];
    int* blockToUnique;
    BlockDefinition* uniqueBlocks;
    UniqueBlocksResult* result;

    n = seq->numBlocks;
    matrix = (float (*)[BLOCK_COMPARE_COLS])ALLOC(n * sizeof(float[BLOCK_COMPARE_COLS]));
    blockToUnique = (int*)ALLOC(n * sizeof(int));
    uniqueBlocks = (BlockDefinition*)ALLOC(n * sizeof(BlockDefinition));

    for (i = 0; i < n; i++) {
        col = 0;
        blockRow = matrix[i];

        /* duration */
        blockRow[col++] = seq->blockLibrary[i][0];

        /* RF info */
        rf_idx = (int)seq->blockLibrary[i][1] - 1;
        if (rf_idx >= 0 && rf_idx < seq->rfLibrarySize) {
            rfData = seq->rfLibrary[rf_idx];
            blockRow[col++] = rfData[1]; /* mag_id */
            blockRow[col++] = rfData[2]; /* phase_id */
            blockRow[col++] = rfData[3]; /* shape_id */
            blockRow[col++] = rfData[5]; /* delay */
        } else {
            blockRow[col++] = 0;
            blockRow[col++] = 0;
            blockRow[col++] = 0;
            blockRow[col++] = 0;
        }

        /* Grad info for x, y, z */
        for (axis = 0; axis < 3; axis++) {
            grad_idx = (int)seq->blockLibrary[i][2 + axis] - 1;
            if (grad_idx >= 0 && grad_idx < seq->gradLibrarySize) {
                gradData = seq->gradLibrary[grad_idx];
                type = (int)gradData[0];
                blockRow[col++] = (float)type;
                if (type == 0) { /* TRAP */
                    blockRow[col++] = gradData[2]; /* rise */
                    blockRow[col++] = gradData[3]; /* flat */
                    blockRow[col++] = gradData[4]; /* fall */
                    blockRow[col++] = gradData[5]; /* delay */
                } else { /* ARB */
                    blockRow[col++] = gradData[4]; /* shape_id */
                    blockRow[col++] = gradData[5]; /* time_id */
                    blockRow[col++] = 0;
                    blockRow[col++] = gradData[6]; /* delay */
                }
            } else {
                blockRow[col++] = 0;
                blockRow[col++] = 0;
                blockRow[col++] = 0;
                blockRow[col++] = 0;
                blockRow[col++] = 0;
            }
        }

        /* ADC info */
        adc_idx = (int)seq->blockLibrary[i][5] - 1;
        if (adc_idx >= 0 && adc_idx < seq->adcLibrarySize) {
            adcData = seq->adcLibrary[adc_idx];
            blockRow[col++] = adcData[0]; /* num samples */
            blockRow[col++] = adcData[1]; /* dwell */
            blockRow[col++] = adcData[2]; /* delay */
        } else {
            blockRow[col++] = 0;
            blockRow[col++] = 0;
            blockRow[col++] = 0;
        }

        /* Trigger info via extension chain */
        blockRow[col++] = 0;
        blockRow[col++] = 0;
        found = 0;
        if (seq->triggerLibrarySize > 0) {
            ext_idx = (int)seq->blockLibrary[i][6];
            while (ext_idx > 0 && ext_idx <= seq->extensionsLibrarySize) {
                extData = seq->extensionsLibrary[ext_idx - 1];
                type = (int)extData[0];
                ref  = (int)extData[1] - 1;
                if (type == 1 && ref >= 0 && ref < seq->triggerLibrarySize) { /* 1 = trigger */
                    trigData = seq->triggerLibrary[ref];
                    blockRow[col - 2] = trigData[2]; /* type */
                    blockRow[col - 1] = trigData[3]; /* channel */
                    found = 1;
                    break;
                }
                ext_idx = (int)extData[2];
            }
        }
    }

    /* Sort matrix */
    qsort(matrix, n, sizeof(float[BLOCK_COMPARE_COLS]), compare_block_rows);

    /* Find unique rows */
    numUnique = 0;
    for (i = 0; i < n; i++) {
        if (i == 0 || compare_block_rows(matrix[i], matrix[i - 1]) != 0) {
            uniqueBlocks[numUnique].duration = (int)matrix[i][0];
            uniqueBlocks[numUnique].rfID     = (int)seq->blockLibrary[i][1];
            uniqueBlocks[numUnique].gxID     = (int)seq->blockLibrary[i][2];
            uniqueBlocks[numUnique].gyID     = (int)seq->blockLibrary[i][3];
            uniqueBlocks[numUnique].gzID     = (int)seq->blockLibrary[i][4];
            uniqueBlocks[numUnique].adcID    = (int)seq->blockLibrary[i][5];
            numUnique++;
        }
    }

    /* Assign blockToUnique */
    for (i = 0; i < n; i++) {
        for (j = 0; j < numUnique; j++) {
            if (compare_block_rows(matrix[i], matrix[j]) == 0) {
                blockToUnique[i] = j;
                break;
            }
        }
    }

    result = (UniqueBlocksResult*)ALLOC(sizeof(UniqueBlocksResult));
    result->uniqueBlocks = uniqueBlocks;
    result->numUniqueBlocks = numUnique;
    result->blockToUnique = blockToUnique;

    FREE(matrix);
    return result;
}

/**
 * @brief Frees memory allocated for a UniqueBlocksResult structure.
 *
 * @param result Pointer to UniqueBlocksResult to free.
 */
void freeUniqueBlocksResult(UniqueBlocksResult* result)
{
    if (!result) return;
    if (result->uniqueBlocks) FREE(result->uniqueBlocks);
    if (result->blockToUnique) FREE(result->blockToUnique);
    FREE(result);
}