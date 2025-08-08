#ifndef RASTER_UTILS_H
#define RASTER_UTILS_H

/**
 * @brief Adjust arbitrary waveform samples to align with the target raster.
 *
 * @param[in] samples Original waveform samples.
 * @param[in] numSamples Number of original samples.
 * @param[in] waveformRaster Original waveform raster.
 * @param[in] targetRaster Target hardware raster.
 * @param[out] adjustedSamples Adjusted waveform samples.
 * @param[out] adjustedNumSamples Number of adjusted samples.
 */
void adjustArbitraryWaveform(float* samples, int numSamples, float waveformRaster, float targetRaster, float** adjustedSamples, int* adjustedNumSamples);

/**
 * @brief Adjust trapezoid corners to align with the target raster.
 *
 * @param[in,out] corners Array of corner positions.
 * @param[in] numCorners Number of corners.
 * @param[in] targetRaster Target hardware raster.
 */
void adjustTrapezoidCorners(float* corners, int numCorners, float targetRaster);

#endif /* RASTER_UTILS_H */
