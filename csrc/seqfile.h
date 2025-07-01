/**
 * @file seqfile.h
 * @brief Public API for parsing Pulseq files.
 *
 */
#ifndef SEQFILE_H
#define SEQFILE_H

#include <string.h>
#include "constants.h"
#include "event.h"


typedef struct {
    char* fpath;                   /* @brief sequence (.seq) file path */
    int version_combined;          /* @brief version_combined = 1000000 * version_major + 1000 * version_minor + version_revision */
    int version_major;
    int version_minor;
    int version_revision;

    int adcLibrarySize;
    float (*adcLibrary)[8];        /* @brief num dwell delay freqPPM phasePPM freq phase phase_id */

    int blockLibrarySize;
    int (*blockLibrary)[6];        /* @brief  duration rf gx gy gx ext */ 

    int extensionsLibrarySize;
    int (*extensionsLibrary)[3];   /* @brief type ref next_id */

    int gradLibrarySize;
    float (*gradLibrary)[7];       /* @brief type amp rise/first flat/last fall/shape_id delay/time_id unused/delay */

    int labelincLibrarySize;
    int (*labelincLibrary)[2];     /* @brief inc labelstring */

    int labelsetLibrarySize;
    int (*labelsetLibrary)[2];     /* @brief set labelstring */

    int rfLibrarySize;
    float (*rfLibrary)[10];        /* @brief amp mag_id phase_id time_id center delay freqPPM phasePPM freq phase */      

    int rfShimLibrarySize;
    float** rfShimLibrary;         /* @brief num_chan magn_c1 phase_c1 magn_c2 phase_c2 ... */
    
    int rotationLibrarySize;
    float (*rotationLibrary)[4];   /* @brief RotQuat0 RotQuatX RotQuatY RotQuatZ */

    int shapeLibrarySize;
    ShapeArbitrary** shapeLibrary;

    int softDelayLibrarySize;
    int (*softDelayLibrary)[3];    /* @brief numID offset factor */ 
    char (*softDelayHintLibrary)[SOFT_DELAY_HINT_LENGTH];

    int triggerLibrarySize;
    int (*triggerLibrary)[4];      /* @brief duration delay type channel */
} SeqFile;

#endif /* SEQFILE_H */