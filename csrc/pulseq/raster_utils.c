#include "raster_utils.h"
#include <math.h>
#include <stdlib.h>

void adjustArbitraryWaveform(float* samples, int numSamples, float waveformRaster, float targetRaster, float** adjustedSamples, int* adjustedNumSamples) {
    int i;
    float scaleFactor = targetRaster / waveformRaster;

    if (scaleFactor == (int)scaleFactor) {
        /* Integer submultiple: reduce samples */
        *adjustedNumSamples = numSamples / (int)scaleFactor;
        *adjustedSamples = (float*)malloc(*adjustedNumSamples * sizeof(float));
        for (i = 0; i < *adjustedNumSamples; i++) {
            (*adjustedSamples)[i] = samples[i * (int)scaleFactor];
        }
    } else {
        /* Perform linear interpolation */
        int newNumSamples = (int)ceil(numSamples * scaleFactor);
        *adjustedSamples = (float*)malloc(newNumSamples * sizeof(float));
        for (i = 0; i < newNumSamples; i++) {
            float t = i / scaleFactor;
            int t0 = (int)floor(t);
            int t1 = t0 + 1;
            float alpha = t - t0;
            (*adjustedSamples)[i] = (1 - alpha) * samples[t0] + alpha * samples[t1];
        }
        *adjustedNumSamples = newNumSamples;
    }
}

void adjustTrapezoidCorners(float* corners, int numCorners, float targetRaster) {
    int i;
    for (i = 0; i < numCorners; i++) {
        corners[i] = round(corners[i] / targetRaster) * targetRaster;
    }
}
