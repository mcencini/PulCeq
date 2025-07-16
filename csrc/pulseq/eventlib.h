/**
 * @file eventlib.h
 * @brief Public API for parsing EventLibraries from Pulseq files.
 *
 */
#ifndef EVENTLIB_H
#define EVENTLIB_H

#include "event.h"
#include "readlib.h"
#include "scanfile.h"

typedef struct SeqFile SeqFile;  /* Forward declaration */

/**
 * @brief Read the Definitions library from sequence file.
 *
 * Initializes and reads the [DEFINITIONS] section.
 * Sets seq.isDefinitionsLibraryParsed to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and definitionsLibrary info
 * @param[in] f Pointer to SeqFile handle.
 */
void readDefinitionsLibrary(SeqFile* seq, FILE* f);

/**
 * @brief Read the Block library from sequence file.
 *
 * Initializes and reads the [BLOCKS] section.
 * Sets seq.isBlockLibraryParsed to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and blockLibrary info
 * @param[in] f Pointer to SeqFile handle.
 */
void readBlockLibrary(SeqFile* seq, FILE* f);

/**
 * @brief Read the RF library from sequence file.
 *
 * Initializes and reads the [RF] section.
 * Sets seq.isRfLibraryParsed to 1 after successful load.
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
 * Sets seq.isAdcLibraryParsed to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and adcLibrary info
 * @param[in] f Pointer to SeqFile handle.
 */
void readAdcLibrary(SeqFile* seq, FILE* f);

/**
 * @brief Read the Shapes library from sequence file.
 *
 * Initializes and reads the [SHAPES] section.
 * Sets seq.isShapesLibraryParsed to 1 after successful load.
 *
 * @param[in,out] seq Sequence struct containing filePath and shapesLibrary info
 * @param[in] f Pointer to SeqFile handle.
 */
void readShapesLibrary(SeqFile* seq, FILE* f);

#endif /* EVENTLIB_H */