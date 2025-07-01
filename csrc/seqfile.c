/**
 * @file seqfile.c
 * @brief Implementation of Pulseq seqfile reading.
 *
 */
#include "pulSeg.h"
#include "seqfile.h"


/**
 * @brief Map string label names to integer label codes.
 * 
 * @param[in] label The string representation of the label (e.g., "LIN").
 * @return int The corresponding numeric code, or -1 if not recognized.
 */
int parseLabelType(const char *label) {
    if (label == NULL) return -1;

    struct {
        const char *name;
        int value;
    } static const labelTable[] = {
        { "SLC", SLC },
        { "SEG", SEG },
        { "REP", REP },
        { "AVG", AVG },
        { "SET", SET },
        { "ECO", ECO },
        { "PHS", PHS },
        { "LIN", LIN },
        { "PAR", PAR },
        { "ACQ", ACQ },
        { "TRID", TRID },
        { "NAV", NAV },
        { "REV", REV },
        { "SMS", SMS },
        { "REF", REF },
        { "IMA", IMA },
        { "NOISE", NOISE },
        { "PMC", PMC },
        { "NOROT", NOROT },
        { "NOPOS", NOPOS },
        { "NOSCL", NOSCL },
        { "ONCE", ONCE },
        { NULL, -1 }  /* Sentinel */
    };

    for (int i = 0; labelTable[i].name != NULL; i++) {
        if (strcmp(label, labelTable[i].name) == 0) {
            return labelTable[i].value;
        }
    }

    return -1;  /* Not found */
}