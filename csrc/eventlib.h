/**
 * @file eventlib.h
 * @brief Public API for parsing EventLibraries from Pulseq files.
 *
 */
#ifndef EVENTLIB_H
#define EVENTLIB_H

struct SeqFile;  /* Forward declaration */

/**
 * @brief Read the ADC library from sequence file.
 *
 * Initializes and reads the [ADC] section.
 * Sets seq.isAdcLibrary to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and adcLibrary info
 */
void readAdcLibrary(SeqFile* seq);

/**
 * @brief Read [EXTENSIONS] section from the sequence file.
 *
 * This function parses the [EXTENSIONS] section, which maps block-level numerical
 * extension IDs to a specific extension type. The mapping is stored as an array of
 * ExtensionEntry records, each containing an ID and a type.
 *
 * Each line inside [EXTENSIONS] should follow the format:
 * - `EXTENSION_NAME TYPE_ID`
 * - e.g. `TRIGGER 1`
 *
 * The numerical ID (TYPE_ID) is used in [BLOCKS] to reference a particular extension.
 *
 * @param seq Pointer to SeqFile structure that holds extensions array and file path.
 */
void readExtensionsLibrary(SeqFile* seq);

/**
 * @brief Read the GRAD and TRAP libraries from sequence file.
 *
 * Initializes and reads the [GRAD] and [TRAP] sections.
 * Applies a flag of 1 to identify these as gradient blocks.
 *
 * @param[in,out] seq Sequence struct containing filePath and gradLibrary info
 */
void readGradLibrary(SeqFile* seq);

/**
 * @brief Read the RF library from sequence file.
 *
 * Initializes and reads the [RF] section.
 * Sets seq.isRfLibrary to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and rfLibrary info
 */
void readRfLibrary(SeqFile* seq);

#endif /* EVENTLIB_H */