/**
 * @file seqfile.h
 * @brief Public API for parsing Pulseq files.
 *
 */
#ifndef SEQFILE_H
#define SEQFILE_H

#include "pulseq/constants.h"
#include "pulseq/event.h"

/**
 * @struct SectionOffset
 * @brief Line positions of the different sections of a Pulseq file.
 * 
 */
typedef struct {
    long scan_cursor;
    long version;
    long definitions;
    long blocks;
    long rf;
    long grad;
    long trap;
    long adc;
    long extensions;
    long triggers;
    long rotations;
    long labelset;
    long labelinc;
    long delays;
    long rfshim;
    long shapes;
} SectionOffsets;

/**
 * @struct Definition
 * @brief Custom definition for sequence description.
 * 
 * @note Values are stored as char, and must be casted to the correct datatype according to interpreter needs.
 */
typedef struct {
    char name[DEFINITION_NAME_LENGTH];
    int valueSize;
    char** value;
} Definition;


/**
 * @struct RfShimEntry
 * @brief Custom structure for RF shim description. Needed to handle variable nChannels case.
 * 
 */
typedef struct {
    int nChannels;   /**< Number of channels */
    float* values;   /**< Pointer to array of size 2 * nChannels (mag1, phase1, mag2, phase2, ...) */
} RfShimEntry;


/**
 * @struct SeqFile
 * @brief Represents a parsed sequence file containing various libraries and metadata.
 * 
 * This structure stores the data parsed from a sequence (.seq) file, including version
 * information, multiple specialized data libraries, and flags indicating which libraries
 * have been successfully parsed.
 * 
 * @note Many library pointers point to dynamically allocated arrays; 
 *       corresponding *_Size fields indicate their lengths.
 */
typedef struct {
    char* filePath;                 /**< @brief Path to the sequence (.seq) file. */
    SectionOffsets offsets;         /**< @brief Line position of each section. */

    int versionCombined;            /**< @brief Combined version number calculated as:
                                         1000000 * versionMajor + 1000 * versionMinor + versionRevision. */
    int versionMajor;               /**< @brief Major version number. */
    int versionMinor;               /**< @brief Minor version number. */
    int versionRevision;            /**< @brief Revision version number. */

    int isDefinitionsLibraryParsed; /**< @brief Flag indicating if the definitions library was parsed successfully. */
    int numDefinitions;             /**< @brief Number of definitions parsed. */
    Definition* definitionsLibrary; /**< @brief Array of parsed definitions. */

    int isBlockLibraryParsed;       /**< @brief Flag indicating if the block library was parsed. */
    int numBlocks;                  /**< @brief Number of block entries. */
    int (*blockLibrary)[6];         /**< @brief Block library data with columns:
                                         duration, rf, gx, gy, gz, ext. */

    int isRfLibraryParsed;          /**< @brief Flag indicating if the RF library was parsed. */
    int rfLibrarySize;              /**< @brief Number of RF entries. */
    float (*rfLibrary)[10];         /**< @brief RF library data with columns:
                                         amp, mag_id, phase_id, time_id, center, delay,
                                         freqPPM, phasePPM, freq, phase. */

    int isGradLibraryParsed;        /**< @brief Flag indicating if the gradient library was parsed. */
    int gradLibrarySize;            /**< @brief Number of gradient entries. */
    float (*gradLibrary)[7];        /**< @brief Gradient library data with columns:
                                         type, amp, rise/first flat, last fall, shape_id, delay, time_id, unused/delay. */

    int isAdcLibraryParsed;         /**< @brief Flag indicating if the ADC library was parsed. */
    int adcLibrarySize;             /**< @brief Number of ADC entries. */
    float (*adcLibrary)[8];         /**< @brief ADC library data with columns:
                                        num, dwell, delay, freqPPM, phasePPM, freq, phase, phase_id. */

    int isExtensionsLibraryParsed;  /**< @brief Flag indicating if the extensions library was parsed. */
    int extensionsLibrarySize;      /**< @brief Number of extension entries. */
    int (*extensionsLibrary)[3];    /**< @brief Extensions library data with columns:
                                          type, ref, next_id. */

    int triggerLibrarySize;         /**< @brief Number of trigger entries. */
    int (*triggerLibrary)[4];       /**< @brief Trigger library data with columns:
                                         duration, delay, type, channel. */

    int rotationLibrarySize;        /**< @brief Number of rotation entries. */
    float (*rotationLibrary)[4];    /**< @brief Rotation quaternion data with columns:
                                         RotQuat0, RotQuatX, RotQuatY, RotQuatZ. */

    int isLabelDefined[22];         /**< For each type of Label in constants.h, flags whether it was defined or not in the given SeqFile */
    int labelsetLibrarySize;        /**< @brief Number of label set entries. */
    int (*labelsetLibrary)[2];      /**< @brief Label set data with columns:
                                         set, labelstring index. */

    int labelincLibrarySize;        /**< @brief Number of label increment entries. */
    int (*labelincLibrary)[2];      /**< @brief Label increment data with columns:
                                         increment, labelstring index. */

    int softDelayLibrarySize;       /**< @brief Number of soft delay entries. */
    int (*softDelayLibrary)[3];     /**< @brief Soft delay data with columns:  numID, offset, factor. */

    int rfShimLibrarySize;          /**< @brief Number of RF shim entries. */
    RfShimEntry* rfShimLibrary;     /**< @brief RF shim data; per-channel magnitude and phase arrays:
                                         magn_c1, phase_c1, magn_c2, phase_c2, ... */

    int extensionMap[8];            /**< @brief Map assigning to each EXT Enum its actual numerical ID written in SeqFile */
    int extensionLUTSize;           /**< @brief Size of look-up table to retrieve from a given extension numerical ID the underlying Enum (type) */
    int *extensionLUT;              /**< @brief Look-up table to retrieve from a given extension numerical ID the underlying Enum (type) */

    int isShapesLibraryParsed;      /**< @brief Flag indicating if the shape library was parsed. */
    int shapesLibrarySize;          /**< @brief Number of shape entries. */
    ShapeArbitrary* shapesLibrary;  /**< @brief Array of pointers to arbitrary shape structures. */

} SeqFile;

/**
 * @brief Initialize SeqFile struct.
 * 
 * @param[in] filePath Path on disk of sequence file (.seq).
 * @return Initializes SeqFile structure  
 */
SeqFile* seqFile(char* filePath);

/**
 * @brief Destroy SeqFile struct.
 * 
 * @param[in] seq The SeqFile structure to be destroyed.
 */
void seqFileFree(SeqFile* seq);

/**
 * @brief Reset SeqFile struct.
 * 
 * Reset all internal pointers to NULL, their size to 0,
 * and is*Parsed flags to 0.
 * 
 * @param[in] seq The SeqFile structure to be reset.
 */
void seqFileReset(SeqFile* seq);

/**
 * @brief Read [DEFINITIONS] section from a sequence file into the definitions table.
 *
 * Each line inside the [DEFINITIONS] section is expected to contain a key-value pair:
 * - Format: `key value` (separated by space or tabs)
 * - Lines beginning with `#` or empty lines are skipped
 * - Section ends when a new section header (e.g., [BLOCKS], [RF], etc.) is encountered
 *
 * This function updates `seq.definitionsLibrary` and `seq.numDefinitions`.
 *
 * @param seq The SeqFile structure containing the file path and output table.
 */
void readDefinitions(SeqFile* seq);

void readLibraries(SeqFile* seq, int readBlocks);

/* SeqBlock getBlock(SeqFile* seq, int blockIndex); */

#endif /* SEQFILE_H */