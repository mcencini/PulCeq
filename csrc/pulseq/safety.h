#ifndef SAFETY_H
#define SAFETY_H

#include "seqfile.h"

/**
 * @brief Compute the maximum gradient amplitude across all gradient events in the sequence.
 *
 * @param[in] seq The sequence file containing the gradLibrary.
 * @return float The maximum gradient amplitude (in T/m).
 */
float computeMaxGradientAmplitude(const SeqFile* seq);

/**
 * @brief Compute the maximum slew rate across all gradient events in the sequence.
 *
 * @param[in] seq The sequence file containing the gradLibrary and shapesLibrary.
 * @return float The maximum slew rate across all gradient events (in T/m/s).
 */
float computeMaxSlewRate(const SeqFile* seq);

/**
 * @brief Compute the maximum B1 amplitude across all RF events in the sequence.
 *
 * @param[in] seq The sequence file containing the rfLibrary.
 * @return float The maximum B1 amplitude (in arbitrary units).
 */
float computeMaxB1Amplitude(const SeqFile* seq);

#endif /* SAFETY_H */
