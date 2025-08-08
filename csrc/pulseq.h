/**
 * @file pulseq.h
 * @brief Public API for Pulseq file parsing and block handling.
 */
#ifndef PULSEQ_H
#define PULSEQ_H

#include "pulseq/seqfile.h"
#include "pulseq/block.h"
#include "pulseq/event.h"
#include "pulseq/label.h"
#include "pulseq/unique_sequence.h"
#include "pulseq/check_sequence.h"
#include "pulseq/ui_config.h"

/* Type definitions for public API */
typedef LabelEvent pulseq_LabelEvent;
typedef LabelLimits pulseq_LabelLimits;
typedef UIConfig pulseq_UIConfig;

/* Public API */
int pulseq_seqFile(char* filePath, pulseq_SeqFile* seq);
void pulseq_seqFileFree(pulseq_SeqFile* seq);
void pulseq_seqFileReset(pulseq_SeqFile* seq);
void pulseq_readDefinitions(pulseq_SeqFile* seq);
void pulseq_readLibraries(pulseq_SeqFile* seq, int readBlocks);
void pulseq_readSeq(pulseq_SeqFile* seq, int forceDecompression);

int pulseq_seqBlock(pulseq_SeqBlock* block);
void pulseq_seqBlockFree(pulseq_SeqBlock* block);
int pulseq_getBlock(const pulseq_SeqFile* seq, int blockIndex, int parseExtensions, pulseq_SeqBlock* block);

int pulseq_getUniqueSeq(pulseq_SeqFile* uniqueSeq, const pulseq_SeqFile* seq);

/**
 * @brief Compute the maximum gradient amplitude across all gradient events in the sequence.
 *
 * @param[in] seq The sequence file containing the gradLibrary.
 * @return float The maximum gradient amplitude (in T/m).
 */
float pulseq_computeMaxGradientAmplitude(const pulseq_SeqFile* seq);

/**
 * @brief Compute the maximum slew rate across all gradient events in the sequence.
 *
 * @param[in] seq The sequence file containing the gradLibrary and shapesLibrary.
 * @return float The maximum slew rate across all gradient events (in T/m/s).
 */
float pulseq_computeMaxSlewRate(const pulseq_SeqFile* seq);

/**
 * @brief Compute the maximum B1 amplitude across all RF events in the sequence.
 *
 * @param[in] seq The sequence file containing the rfLibrary.
 * @return float The maximum B1 amplitude (in arbitrary units).
 */
float pulseq_computeMaxB1Amplitude(const pulseq_SeqFile* seq);

/**
 * @brief Adjust waveforms and trapezoid corners to align with the target hardware raster.
 *
 * @param[in,out] seq The sequence file containing waveforms and gradient events.
 * @param[in] targetRaster The target hardware raster (in microseconds).
 */
void pulseq_adjustWaveforms(pulseq_SeqFile* seq, float targetRaster);

#endif /* PULSEQ_H */
