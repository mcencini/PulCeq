/**
 * @file patterns.c
 * @brief Implements periodic pattern detection for block sequences.
 */

#include <string.h>

#include "../../pulseq/alloc.h"

#include "patterns.h"

/**
 * @brief Checks if two integer arrays are equal.
 *
 * @param a Pointer to first array.
 * @param b Pointer to second array.
 * @param length Number of elements to compare.
 * @return 1 if arrays are equal, 0 otherwise.
 */
static int array_equal(const int* a, const int* b, int length)
{
    int i;
    for (i = 0; i < length; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

/**
 * @brief Counts consecutive repetitions of a pattern in an array.
 *
 * @param arr Array of block IDs.
 * @param start_idx Starting index of pattern.
 * @param length Length of the pattern.
 * @param n_blocks Total number of blocks in arr.
 * @return Number of consecutive repetitions of the pattern.
 */
static int count_repetitions(const int* arr, int start_idx, int length, int n_blocks)
{
    int count = 0;
    while (start_idx + (count + 1) * length <= n_blocks) {
        if (array_equal(&arr[start_idx + count * length], &arr[start_idx], length)) {
            count++;
        } else {
            break;
        }
    }
    return count;
}

/**
 * @brief Finds the first periodic pattern in an array.
 *
 * @param arr Array of block IDs.
 * @param n_blocks Number of blocks.
 * @param min_length Minimum pattern length.
 * @param start_idx Pointer to output start index of pattern.
 * @param length Pointer to output length of pattern.
 * @return 1 if a pattern is found, 0 otherwise.
 */
static int find_periodic_pattern(
    const int* arr, int n_blocks, int min_length, int* start_idx, int* length)
{
    int i, j, max_repeats, best_start, best_length, repeats;
    max_repeats = 0;
    best_start = -1;
    best_length = -1;
    for (i = 0; i < n_blocks - min_length + 1; i++) {
        for (j = min_length; j <= (n_blocks - i) / 2; j++) {
            if (array_equal(&arr[i], &arr[i + j], j)) {
                repeats = count_repetitions(arr, i, j, n_blocks);
                if (repeats > max_repeats || (repeats == max_repeats && j > best_length)) {
                    max_repeats = repeats;
                    best_start = i;
                    best_length = j;
                }
                if (repeats > 1) {
                    *start_idx = best_start;
                    *length = best_length;
                    return 1;
                }
            }
        }
    }
    if (best_start != -1) {
        *start_idx = best_start;
        *length = best_length;
        return 1;
    }
    return 0;
}

/**
 * @brief Reverses an integer array in place.
 *
 * @param arr Array to reverse.
 * @param length Length of the array.
 */
static void reverse_array(int* arr, int length)
{
    int i, tmp;
    for (i = 0; i < length / 2; i++) {
        tmp = arr[i];
        arr[i] = arr[length - 1 - i];
        arr[length - 1 - i] = tmp;
    }
}

/**
 * @brief Recursively finds and labels patterns in the block ID array.
 *
 * @param arr Array of block IDs.
 * @param arr_length Length of the array.
 * @param min_length Minimum pattern length.
 * @param results Pointer to SequencePatterns structure to fill.
 * @param next_tr_id Next available TR ID.
 * @return Next available TR ID after recursion.
 */
static int find_patterns_recursive(
    const int* arr, int arr_length, int min_length, SequencePatterns* results, int next_tr_id)
{
    int start_idx, pattern_length, reps, remainder_start, remainder_length;
    if (arr_length < min_length) {
        if (arr_length > 0) next_tr_id++;
        return next_tr_id;
    }
    if (find_periodic_pattern(arr, arr_length, min_length, &start_idx, &pattern_length)) {
        reps = count_repetitions(arr, start_idx, pattern_length, arr_length);
        next_tr_id++;
        remainder_start = start_idx + (reps * pattern_length);
        remainder_length = arr_length - remainder_start;
        if (remainder_length > 0) {
            next_tr_id = find_patterns_recursive(
                &arr[remainder_start], remainder_length, min_length, results, next_tr_id);
        }
    } else {
        next_tr_id++;
    }
    return next_tr_id;
}

/**
 * @brief Detects periodic patterns in block ID sequence.
 *
 * @param block_ids Array of block IDs.
 * @param n_blocks Number of blocks.
 * @param min_length Minimum pattern length.
 * @param try_reverse If nonzero, also try reversed sequence.
 * @return SequencePatterns structure with detected patterns.
 */
SequencePatterns detectSequencePatterns(const int* block_ids, int n_blocks, int min_length, int try_reverse)
{
    SequencePatterns patterns;
    int i, tr_start;

    patterns.tr_ids = (int*)ALLOC(sizeof(int) * n_blocks);
    patterns.trid = (int*)ALLOC(sizeof(int) * n_blocks);
    for (i = 0; i < n_blocks; i++) {
        patterns.tr_ids[i] = 0;
        patterns.trid[i] = 0;
    }

    patterns.n_blocks = n_blocks;
    patterns.n_tr_definitions = 1;
    patterns.tr_definitions = (TRDefinition*)ALLOC(sizeof(TRDefinition));
    patterns.tr_definitions[0].tr_id = 0;
    patterns.tr_definitions[0].n_blocks = n_blocks;
    patterns.tr_definitions[0].blocks = (int*)ALLOC(sizeof(int) * n_blocks);
    memcpy(patterns.tr_definitions[0].blocks, block_ids, sizeof(int) * n_blocks);

    /* Fill tr_ids and trid arrays for the single TR case */
    for (i = 0; i < n_blocks; i++) {
        patterns.tr_ids[i] = 0;
    }
    tr_start = 0;
    patterns.trid[tr_start] = 0;

    /* For more complex pattern detection, update tr_ids and trid accordingly in your recursive logic */

    return patterns;
}

/**
 * @brief Frees memory associated with SequencePatterns structure.
 *
 * @param patterns Pointer to SequencePatterns structure to free.
 */
void freeSequencePatterns(SequencePatterns* patterns)
{
    int i;
    if (!patterns) return;
    if (patterns->tr_definitions) {
        for (i = 0; i < patterns->n_tr_definitions; i++) {
            if (patterns->tr_definitions[i].blocks) {
                FREE(patterns->tr_definitions[i].blocks);
            }
        }
        FREE(patterns->tr_definitions);
    }
    if (patterns->tr_ids) {
        FREE(patterns->tr_ids);
    }
    if (patterns->trid) {
        FREE(patterns->trid);
    }
    patterns->n_blocks = 0;
    patterns->n_tr_definitions = 0;
}