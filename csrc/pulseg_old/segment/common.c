/**
 * @file common.c
 * @brief Implementation of common segment functions.
 */

#include "../../pulseq/constants.h"

#include "common.h"

int hasTridLabels(const pulseq_SeqFile* seq)
{
    int i;
    
    /* Check if TRID labels are defined in the sequence */
    if (!seq->isExtensionsLibraryParsed || seq->labelsetLibrarySize == 0) {
        return 0;
    }
    
    /* Check if any TRID label is used */
    for (i = 0; i < seq->labelsetLibrarySize; i++) {
        if (seq->labelsetLibrary[i][0] == TRID) { /* TRID defined in constants.h */
            return 1;
        }
    }
    
    return 0;
}
