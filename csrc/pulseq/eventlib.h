/**
 * @file eventlib.h
 * @brief Public API for parsing EventLibraries from Pulseq files.
 *
 */
#ifndef EVENTLIB_H
#define EVENTLIB_H

#include "readlib.h"
#include "scanfile.h"

typedef struct SeqFile SeqFile;  /* Forward declaration */

/**
 * @brief Read the RF library from sequence file.
 *
 * Initializes and reads the [RF] section.
 * Sets seq.isRfLibrary to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and rfLibrary info
 * @param[in] f Pointer to SeqFile handle.
 */
void readRfLibrary(SeqFile* seq, FILE* f);

/**
 * @brief Read the GRAD and TRAP libraries from sequence file.
 *
 * Initializes and reads the [GRAD] and [TRAP] sections.
 * Applies a flag of 1 to identify these as gradient blocks.
 *
 * @param[in,out] seq Sequence struct containing filePath and gradLibrary info
* @param[in] f Pointer to SeqFile handle.
 */
void readGradLibrary(SeqFile* seq, FILE* f);

/**
 * @brief Read the ADC library from sequence file.
 *
 * Initializes and reads the [ADC] section.
 * Sets seq.isAdcLibrary to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and adcLibrary info
 * @param[in] f Pointer to SeqFile handle.
 */
void readAdcLibrary(SeqFile* seq, FILE* f);

#endif /* EVENTLIB_H */