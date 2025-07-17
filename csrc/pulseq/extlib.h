/**
 * @file eventlib.h
 * @brief Public API for parsing EventLibraries from Pulseq files.
 *
 */
#ifndef EXTLIB_H
#define EXTLIB_H

#include "readlib.h"
#include "scanfile.h"

#ifndef SEQFILE_H
typedef struct SeqFile SeqFile;  /* Forward declaration */
#endif /* SEQFILE_H */

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
 * @param[in, out] seq Pointer to SeqFile structure that holds extensions array and file path.
 * @param[in] f Pointer to SeqFile handle.
 */
void readExtensionsLibrary(SeqFile* seq, FILE* f);

#endif /* EXTLIB_H */