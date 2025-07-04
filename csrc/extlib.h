/**
 * @file eventlib.h
 * @brief Public API for parsing EventLibraries from Pulseq files.
 *
 */
#ifndef EXTLIB_H
#define EXTLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SeqFile;  /* Forward declaration */

/**
 * @brief Read all extension libraries from the sequence file.
 *
 * Scans the file for "extension <NAME> <ID>" lines and dispatches to
 * the corresponding readExtensionX() parser. Populates the extensionMap
 * so that block-level IDs can be efficiently mapped to known types.
 *
 * @param seq Pointer to SeqFile structure
 * @return 0 on success, non-zero on failure
 */
int readExtensions(SeqFile* seq);

#endif /* EXTLIB_H */