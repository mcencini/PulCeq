#include <math.h>
#include <stddef.h>

#include "waveutils.h"

int determineWaveformTypeAndRaster(float* rasterTime, const float** timeArray, const SeqFile* seq, int gradIndex) {
    const float* gradEvent;
    int shape_id;
    int time_id;
    float dt;
    int i;

    /* Initialize outputs */
    *rasterTime = seq->reservedDefinitionsLibrary.gradientRasterTime * 1e-6; /* Convert us to seconds */
    *timeArray = NULL;

    /* Access the gradient event */
    gradEvent = seq->gradLibrary[gradIndex];
    shape_id = (int)gradEvent[4]; /* shape_id */
    time_id = (int)gradEvent[5]; /* time_id */

    if (shape_id == 0) {
        /* Trapezoid */
        return 0;
    } else if (time_id == 0) {
        /* Uniform raster arbitrary waveform with default raster */
        return 1;
    } else if (time_id == -1) {
        /* Oversampled shape, use default raster */
        return 1;
    } else if (time_id > 0) {
        /* Explicit time array */
        *timeArray = seq->shapesLibrary[time_id].samples;
        if (seq->shapesLibrary[time_id].numSamples > 1) {
            dt = (*timeArray)[1] - (*timeArray)[0];
            for (i = 2; i < seq->shapesLibrary[time_id].numSamples; i++) {
                if (fabs((*timeArray)[i] - (*timeArray)[i - 1] - dt) > 1e-6) {
                    /* Non-uniform time array */
                    return 2;
                }
            }
            /* Uniform time array */
            *rasterTime = dt;
            return 1;
        }
    }

    return -1; /* Unknown type */
}
