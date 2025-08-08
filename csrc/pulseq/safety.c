#include "safety.h"
#include "waveutils.h"
#include <math.h>

float computeMaxGradientAmplitude(const SeqFile* seq) {
    int i;
    float maxAmplitude = 0.0f;

    /* Loop over gradLibrary to find the maximum amplitude */
    for (i = 0; i < seq->gradLibrarySize; i++) {
        if (seq->gradLibrary[i][1] > maxAmplitude) {
            maxAmplitude = seq->gradLibrary[i][1];
        }
    }

    return maxAmplitude;
}

float computeMaxSlewRate(const SeqFile* seq) {
    int i;
    float maxSlewRate = 0.0f;

    /* Loop over gradLibrary to compute the maximum slew rate */
    for (i = 0; i < seq->gradLibrarySize; i++) {
        float rasterTime;
        const float* timeArray;
        int waveformType;
        float amp;
        int shape_id;
        const float* shapeSamples;
        int numSamples;
        float maxSlew;
        int j;

        /* Determine waveform type and raster */
        waveformType = determineWaveformTypeAndRaster(&rasterTime, &timeArray, seq, i);
        amp = seq->gradLibrary[i][1]; /* Gradient amplitude */
        shape_id = (int)seq->gradLibrary[i][4]; /* shape_id */
        shapeSamples = seq->shapesLibrary[shape_id].samples;
        numSamples = seq->shapesLibrary[shape_id].numSamples;

        if (waveformType == 0) {
            /* Trapezoid */
            float riseTime = seq->gradLibrary[i][2];
            float fallTime = seq->gradLibrary[i][4];
            maxSlew = fmax(amp / riseTime, amp / fallTime);
        } else if (waveformType == 1) {
            /* Uniform raster arbitrary waveform */
            maxSlew = 0.0f;
            for (j = 1; j < numSamples; j++) {
                float slew = fabs((shapeSamples[j] - shapeSamples[j - 1]) / rasterTime);
                if (slew > maxSlew) {
                    maxSlew = slew;
                }
            }
            maxSlew *= amp;
        } else if (waveformType == 2) {
            /* Non-uniform time array */
            maxSlew = 0.0f;
            for (j = 1; j < numSamples; j++) {
                float dt = timeArray[j] - timeArray[j - 1];
                float slew = fabs((shapeSamples[j] - shapeSamples[j - 1]) / dt);
                if (slew > maxSlew) {
                    maxSlew = slew;
                }
            }
            maxSlew *= amp;
        } else {
            /* Unknown type */
            maxSlew = 0.0f;
        }

        if (maxSlew > maxSlewRate) {
            maxSlewRate = maxSlew;
        }
    }

    return maxSlewRate;
}

float computeMaxB1Amplitude(const SeqFile* seq) {
    int i;
    float maxB1Amplitude = 0.0f;

    /* Loop over rfLibrary to find the maximum rfAmp value */
    for (i = 0; i < seq->rfLibrarySize; i++) {
        if (seq->rfLibrary[i][1] > maxB1Amplitude) {
            maxB1Amplitude = seq->rfLibrary[i][1];
        }
    }

    return maxB1Amplitude;
}
