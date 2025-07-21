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

ShapeArbitrary* decompressShape(ShapeArbitrary* encoded)
{
    int i, rep;
    const float *packed = encoded->samples;
    int numPacked = encoded->numSamples;
    int numSamples = encoded->numUncompressedSamples;
    int countPack = 1;
    int countUnpack = 1;
    float* unpacked;
    ShapeArbitrary *decoded;

    /* Input shape is uncompressed - copy it */
    if (encoded->numSamples == encoded->numUncompressedSamples) {
        decoded = (ShapeArbitrary*)ALLOC(sizeof(ShapeArbitrary));
        if (!decoded) return NULL;
        decoded->numSamples = encoded->numSamples;
        decoded->numUncompressedSamples = encoded->numUncompressedSamples;
        decoded->samples = (float*)ALLOC(sizeof(float) * encoded->numSamples);
        if (!decoded->samples) {
            FREE(decoded);
            return NULL;
        }
        memcpy(decoded->samples, encoded->samples, sizeof(float) * encoded->numSamples);
        return decoded;
    }

    unpacked = (float*) ALLOC(sizeof(float) * numSamples);
    if (unpacked == NULL) {
        return NULL; /* Allocation failed */
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
                return NULL;
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

    decoded = (ShapeArbitrary*) ALLOC(sizeof(ShapeArbitrary));
    if (decoded == NULL) {
        FREE(unpacked);
        return NULL;
    }

    decoded->numSamples = numSamples;
    decoded->numUncompressedSamples = numSamples;
    decoded->samples = unpacked;

    return decoded;
}