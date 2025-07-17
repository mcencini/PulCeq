/**
 * @file scanfile.h
 * @brief Public API for scanning a Sequence file.
 *
 */
#ifndef SCANFILE_H
#define SCANFILE_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "alloc.h"
#include "constants.h"

#ifndef SEQFILE_H
typedef struct SeqFile SeqFile;  /* Forward declaration */
#endif /* SEQFILE_H */

/**
 * @brief Scan for the offset of a target section and update SectionOffsets cache.
 *
 * @param[in,out] sectionOffsets  Output array of length numSections, will be filled with file offsets.
 * @param[in,out] seq         Pointer to SeqFile struct.
 * @param[in] f               Opened file handle (text mode)
 * @param[in] sectionNames    Array of section header strings to find.
 * @param[in] numSections     Number of sections to find.
 * @param[in] parseExtensions Whether to parse all "extension STR_ID NUM_ID " lines.
 */
void getSectionOffsets(long* sectionOffsets, SeqFile* seq, FILE* f, const char** sectionNames, int numSections, int parseExtensions);

#endif /* SCANFILE_H */