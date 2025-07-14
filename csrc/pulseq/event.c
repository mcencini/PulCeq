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

ShapeArbitrary* decompressShape(const ShapeArbitrary* encoded)
{
    if (encoded->numSamples == encoded->numUncompressedSamples) {
        /* Already uncompressed, return as-is */
        return (ShapeArbitrary*) encoded;
    }

    const float *packed = encoded->samples;
    int numPacked = encoded->numSamples;
    int numSamples = encoded->numUncompressedSamples;

    float *unpacked = (float*) ALLOC(sizeof(float) * numSamples);
    if (unpacked == NULL) {
        return NULL; /* Allocation failed */
    }

    int countPack = 1;
    int countUnpack = 1;

    while (countPack < numPacked) {
        if (packed[countPack - 1] != packed[countPack]) {
            unpacked[countUnpack - 1] = packed[countPack - 1];
            countPack++;
            countUnpack++;
        } else {
            int rep = (int)(packed[countPack + 1]) + 2;
            if (fabsf(packed[countPack + 1] + 2 - (float)rep) > 1e-6f) {
                /* Malformed shape compression format */
                FREE(unpacked);
                return NULL;
            }
            for (int i = countUnpack - 1; i <= countUnpack + rep - 2; i++) {
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
    for (int i = 1; i < numSamples; i++) {
        unpacked[i] += unpacked[i - 1];
    }

    ShapeArbitrary *decoded = (ShapeArbitrary*) ALLOC(sizeof(ShapeArbitrary));
    if (decoded == NULL) {
        FREE(unpacked);
        return NULL;
    }

    decoded->numSamples = numSamples;
    decoded->numUncompressedSamples = numSamples;
    decoded->samples = unpacked;

    return decoded;
}