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
 * @param[in,out] seq         Pointer to SeqFile struct.
 * @param[in] f               Opened file handle (text mode)
 */
void getSectionOffsets(SeqFile* seq, FILE* f);

#endif /* SCANFILE_H */