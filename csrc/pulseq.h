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
void pulseq_readSeq(pulseq_SeqFile* seq);

int pulseq_seqBlock(pulseq_SeqBlock* block);
void pulseq_seqBlockFree(pulseq_SeqBlock* block);
int pulseq_getBlock(const pulseq_SeqFile* seq, int blockIndex, int parseExtensions, pulseq_SeqBlock* block);

/** @brief  Get label values for a specific ADC event.
 *  @param[in]  seq              Pointer to a SeqFile struct.
 *  @param[in]  adcIndex         Index of ADC event (0-based, in order of appearance in sequence).
 *  @param[out] labelOut         Pointer to a pre-allocated LabelEvent struct to fill with label values.
 *  @returns    int              1 if successful, 0 if labels couldn't be retrieved.
 */
int pulseq_getLabelsForAdc(const pulseq_SeqFile* seq, int adcIndex, pulseq_LabelEvent* labelOut);

/** @brief  Check if a sequence file is valid (correct version and valid signature).
 *  @param[in]  filePath         Path to the sequence file to check.
 *  @returns    int              1 if the file is valid, 0 otherwise.
 */
int pulseq_checkSeqFile(const char* filePath);

/** @brief  Configure UI based on sequence file content.
 *  @param[out] config           Pointer to UI configuration to update.
 *  @param[in]  filePath         Path to the sequence file.
 */
void pulseq_configureUI(pulseq_UIConfig* config, const char* filePath);

/** @brief  Create a new sequence with unique RF, gradient, and ADC events.
 *         This function copies only RF, gradient, ADC, and block libraries,
 *         and creates a mapping between original block IDs and unique block IDs.
 *  @param[out] uniqueSeq        Pointer to the output SeqFile structure for the unique sequence.
 *  @param[in]  seq              Pointer to the input sequence file.
 *  @returns    int              1 if successful, 0 if failed.
 */
int pulseq_getUniqueSeq(pulseq_SeqFile* uniqueSeq, const pulseq_SeqFile* seq);

/** @brief  Get maximum RF amplitude from a sequence file.
 *  @param[in]  seq              Pointer to the sequence file.
 *  @returns    float            The maximum RF amplitude found in the sequence.
 */
float pulseq_getMaxRFAmplitude(const pulseq_SeqFile* seq);

/** @brief  Get maximum gradient amplitude from a sequence file.
 *  @param[in]  seq              Pointer to the sequence file.
 *  @returns    float            The maximum gradient amplitude found in the sequence.
 */
float pulseq_getMaxGradientAmplitude(const pulseq_SeqFile* seq);

/** @brief  Get maximum slew rate from a sequence file.
 *  @param[in]  seq              Pointer to the sequence file.
 *  @returns    float            The maximum slew rate found in the sequence.
 */
float pulseq_getMaxSlewRate(const pulseq_SeqFile* seq);

/** @brief  Count the number of ADC events with the navigation flag.
 *  @param[in]  seq              Pointer to the sequence file.
 *  @returns    int              The number of ADC events with the navigation flag.
 */
int pulseq_countNavigationADCEvents(const pulseq_SeqFile* seq);

#endif /* PULSEQ_H */
