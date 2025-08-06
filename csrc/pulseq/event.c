/**
 * @file event.c
 * @brief Implementation of Pulseq event handling and shape decompression.
 *
 * Contains definitions of functions declared in event.h, including
 * memory allocation behavior and internal helpers for decoding compressed shapes.
 */

#include <math.h>
#include <string.h>

#include "alloc.h"
#include "event.h"

int decompressShape(ShapeArbitrary* encoded, ShapeArbitrary* result)
{
    int i, rep;
    const float *packed;
    int numPacked, numSamples;
    int countPack = 1;
    int countUnpack = 1;
    float* unpacked;
    
    /* Validate inputs */
    if (!encoded || !result) {
        return 0; /* Invalid inputs */
    }
    
    packed = encoded->samples;
    numPacked = encoded->numSamples;
    numSamples = encoded->numUncompressedSamples;
    
    /* Input shape is uncompressed - copy it */
    if (encoded->numSamples == encoded->numUncompressedSamples) {
        result->numSamples = encoded->numSamples;
        result->numUncompressedSamples = encoded->numUncompressedSamples;
        result->samples = (float*)ALLOC(sizeof(float) * encoded->numSamples);
        if (!result->samples) {
            return 0; /* Allocation failed */
        }
        memcpy(result->samples, encoded->samples, sizeof(float) * encoded->numSamples);
        return 1; /* Success */
    }

    unpacked = (float*) ALLOC(sizeof(float) * numSamples);
    if (unpacked == NULL) {
        return 0; /* Allocation failed */
    }

    while (countPack < numPacked) {
        if (packed[countPack - 1] != packed[countPack]) {
            unpacked[countUnpack - 1] = packed[countPack - 1];
            countPack++;
            countUnpack++;
        } else {
            rep = (int)(packed[countPack + 1]) + 2;
            if (fabsf(packed[countPack + 1] + 2 - (float)rep) > 1e-6f) {
                /* Malformed shape compression format */
                FREE(unpacked);
                return 0; /* Failed */
            }
            for (i = countUnpack - 1; i <= countUnpack + rep - 2; i++) {
                unpacked[i] = packed[countPack - 1];
            }
            countPack += 3;
            countUnpack += rep;
        }
    }

    if (countPack == numPacked) {
        unpacked[countUnpack - 1] = packed[countPack - 1];
    }

    /* Cumulative sum */
    for (i = 1; i < numSamples; i++) {
        unpacked[i] += unpacked[i - 1];
    }

    result->numSamples = numSamples;
    result->numUncompressedSamples = numSamples;
    result->samples = unpacked;

    return 1; /* Success */
}