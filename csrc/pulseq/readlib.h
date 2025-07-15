/**
 * @file readlib.h
 * @brief Public API for parsing EventLibraries from Pulseq files.
 *
 */
#ifndef READLIB_H
#define READLIB_H

#include <stdio.h>

#include "alloc.h"
#include "constants.h"

/**
 * @struct Scale
 * @brief Scale struct used for multiplying parsed library values.
 */
typedef struct {
    int size;         /**< Number of values to scale */
    const float* values; /**< Array of scaling factors */
} Scale;

/**
 * @brief Initialize a combined library array from multiple section offsets.
 *
 * For each section offset, seeks and reads lines to find the maximum index in first column.
 * Allocates and zero-fills a float** array with (maxIndex + 1) rows and numEntries columns.
 *
 * @param[in] f              Opened file handle (text mode)
 * @param[in] offsets        Array of file offsets for sections.
 * @param[in] numSections    Number of sections.
 * @param[out] target        Pointer to float** pointer to store allocated 2D array.
 * @param[out] targetCount   Pointer to int to store allocated row count.
 * @param[in] numEntries     Number of columns (entries) per row.
 *
 * @return 0 on success, non-zero on failure.
 */
int initStandardLibrary(FILE* f, const long* offsets, int numSections, float*** target, int* targetCount, int numEntries);

/**
 * @brief Initialize rf shim library.
 *
 *
 * @param[in] f              Opened file handle (text mode)
 * @param[in] offsets        Array of file offsets for sections.
 * @param[out] target        Pointer to RfShimEntry* pointer to store allocated array.
 * @param[out] targetCount   Pointer to int to store allocated row count.
 *
 * @return 0 on success, non-zero on failure.
 */
int initRfShimLibrary(FILE* f, long offset, RfShimEntry** target, int* targetCount);

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
int readStandardLibrary(FILE* f, long offset, float** target, int targetCount, Scale scale, int flag);

/**
 * @brief Read and parse labelset/labelinc library section from file at given offset.
 *
 * Reads lines starting at offset until next section or EOF.
 * Parses index and values and stores in target array.
 *
 * @param[in] f              Opened file handle (text mode)
 * @param[in] offset         File offset where section starts.
 * @param[in,out] target     Pre-allocated 2D float array.
 * @param[in] targetCount    Number of rows in target.
 * @param[in] isLabelDefined Table to store whether a given label/flag is present or not in SeqFile.
 *
 * @return 0 on success, non-zero on failure.
 */
int readLabelLibrary(FILE* f, long offset, float** target, int targetCount, int* isLabelDefined);

/**
 * @brief Read and parse soft delay library section from file at given offset.
 *
 * Reads lines starting at offset until next section or EOF.
 * Parses index and values and stores in target array.
 *
 * @param[in] f              Opened file handle (text mode)
 * @param[in] offset         File offset where section starts.
 * @param[in,out] target     Pre-allocated 2D float array.
 * @param[in] targetCount    Number of rows in target.
 *
 * @return 0 on success, non-zero on failure.
 */
int readDelayLibrary(FILE* f, long offset, float** target, int targetCount);

/**
 * @brief Read and parse rf shim library section from file at given offset.
 *
 * Reads lines starting at offset until next section or EOF.
 * Parses index and values and stores in target array.
 * If flag >= 0, sets target[index][0] = flag.
 *
 * @param[in] f              Opened file handle (text mode)
 * @param[in] offset         File offset where section starts.
 * @param[in,out] target     Pre-allocated 2D float array.
 * @param[in] targetCount    Number of rows in target.
 *
 * @return 0 on success, non-zero on failure.
 */
int readRfShimLibrary(FILE* f, long offset, RfShimEntry* target, int targetCount);

#endif /* READLIB_H */