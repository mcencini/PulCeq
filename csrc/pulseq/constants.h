/**
 * @file constants.h
 * @brief Constants and Enums.
 *
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEFINITION_NAME_LENGTH 32
#define MAX_LINE_LENGTH 256

/* gradient/RF types */
#define TRAP 1
#define GRAD 2

/*********************************************************************      Extensions     **************************************************************************************/
#define EXT_LIST      0
#define EXT_TRIGGER   1
#define EXT_ROTATION  2
#define EXT_LABELSET  3
#define EXT_LABELINC  4
#define EXT_RF_SHIM   5
#define EXT_DELAY     6
#define EXT_UNKNOWN   7 /* marks the end of the enum, should always be the last */

/*********************************************************************      Labels and Flags     **************************************************************************************/
/****** Label       |     Type     | Data Mapping | Description ***********************************************************************************************************************/
#define SLC 1    /* | counter      |      Yes     | Slice counter (or slab counter for 3D multi-slab sequences) */
#define SEG 2    /* | counter      |      Yes     | Segment counter e.g. for segmented FLASH or EPI */
#define REP 3    /* | counter      |      Yes     | Repetition counter */
#define AVG 4    /* | counter      |      Yes     | Averaging counter */
#define SET 5    /* | counter      |      Yes     | Flexible counter without firm assignment */
#define ECO 6    /* | counter      |      Yes     | Echo counter in multi-echo sequences */
#define PHS 7    /* | counter      |      Yes     | Cardiac phase counter */
#define LIN 8    /* | counter      |      Yes     | Line counter in 2D and 3D acquisitions */
#define PAR 9    /* | counter      |      Yes     | Partition counter; it counts phase encoding steps in the 2nd (through-slab) phase encoding direction in 3D sequences */
#define ACQ 10   /* | counter      |      Yes     | Spectroscopic acquisition counter */
#define TRID 11  /* | counter      |      No      | Marks the beginning of a repeatable module in the sequence (e.g. TR); modules with different timing should be assigned different TRIDs */
#define NAV 12   /* | flag         |      Yes     | Navigator data flag */
#define REV 13   /* | flag         |      Yes     | Flag indicating that the readout direction is reversed */
#define SMS 14   /* | flag         |      Yes     | Simultaneous multi-slice (SMS) acquisition */
#define REF 15   /* | flag         |      Yes     | Parallel imaging flag indicating reference / auto-calibration data */
#define IMA 16   /* | flag         |      Yes     | Parallel imaging flag indicating imaging data within the ACS region */
#define NOISE 17 /* | flag         |      Yes     | Flag for the noise adjust scan e.g for the parallel imaging acceleration */
#define PMC 18   /* | flag         |      No      | Flag for the MoCo/PMC Pulseq version marking blocks that can/should be prospectively corrected for motion */
#define NOROT 19 /* | flag         |      No      | Instructs the interpreter to ignore the rotation of the FOV specified on the UI for the given block(s) */
#define NOPOS 20 /* | flag         |      No      | Instructs the interpreter to ignore the the FOV offset specified on the UI for the given block(s) */
#define NOSCL 21 /* | flag         |      No      | Instructs the interpreter to ignore the scaling of the FOV specified on the UI for the given block(s) */
#define ONCE 22  /* | 3-state flag |      No      | A 3-state flag that instructs the interpreter to alter the sequence when executing multiple repeats as follows: blocks with ONCE==0 are executed on every repetition; ONCE==1: only on the first repetition; ONCE==2: only on the last repetition */

#endif /* CONSTANTS_H */
