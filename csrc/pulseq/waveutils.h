#ifndef WAVEUTILS_H
#define WAVEUTILS_H

#include "seqfile.h"

/**
 * @brief Determine the waveform type and raster for a gradient event.
 *
 * @param[out] rasterTime The raster time for the waveform (in seconds).
 * @param[out] timeArray Pointer to the time array (if applicable, NULL otherwise).
 * @param[in] seq The sequence file containing the gradLibrary, shapesLibrary, and reservedDefinitions.
 * @param[in] gradIndex The index of the gradient event in the gradLibrary.
 * @return int The waveform type (0 = trapezoid, 1 = uniform raster, 2 = non-uniform time array).
 */
int determineWaveformTypeAndRaster(float* rasterTime, const float** timeArray, const SeqFile* seq, int gradIndex);

#endif /* WAVEUTILS_H */
