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

#define DEFINITION_NAME_LENGTH 32
typedef struct {
    char name[DEFINITION_NAME_LENGTH];
    int valueSize;
    float *value;
} Definition;

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

    int versionCombined;            /**< @brief Combined version number calculated as:
                                         1000000 * versionMajor + 1000 * versionMinor + versionRevision. */
    int versionMajor;               /**< @brief Major version number. */
    int versionMinor;               /**< @brief Minor version number. */
    int versionRevision;            /**< @brief Revision version number. */

    int isDefinitionsLibraryParsed; /**< @brief Flag indicating if the definitions library was parsed successfully. */
    int numDefinitions;             /**< @brief Number of definitions parsed. */
    Definition *definitionsLibrary; /**< @brief Array of parsed definitions. */

    int isAdcLibraryParsed;         /**< @brief Flag indicating if the ADC library was parsed. */
    int adcLibrarySize;             /**< @brief Number of ADC entries. */
    float (*adcLibrary)[8];         /**< @brief ADC library data with columns:
                                        num, dwell, delay, freqPPM, phasePPM, freq, phase, phase_id. */

    int isBlockLibraryParsed;       /**< @brief Flag indicating if the block library was parsed. */
    int blockLibrarySize;           /**< @brief Number of block entries. */
    int (*blockLibrary)[6];         /**< @brief Block library data with columns:
                                         duration, rf, gx, gy, gz, ext. */

    int isExtensionsLibraryParsed;  /**< @brief Flag indicating if the extensions library was parsed. */
    int extensionsLibrarySize;      /**< @brief Number of extension entries. */
    int (*extensionsLibrary)[3];    /**< @brief Extensions library data with columns:
                                          type, ref, next_id. */

    int isGradLibraryParsed;        /**< @brief Flag indicating if the gradient library was parsed. */
    int gradLibrarySize;            /**< @brief Number of gradient entries. */
    float (*gradLibrary)[7];        /**< @brief Gradient library data with columns:
                                         type, amp, rise/first flat, last fall, shape_id, delay, time_id, unused/delay. */

    int isLabelincLibraryParsed;    /**< @brief Flag indicating if the label increment library was parsed. */
    int labelincLibrarySize;        /**< @brief Number of label increment entries. */
    int (*labelincLibrary)[2];      /**< @brief Label increment data with columns:
                                         increment, labelstring index. */

    int isLabelsetLibraryParsed;    /**< @brief Flag indicating if the label set library was parsed. */
    int labelsetLibrarySize;        /**< @brief Number of label set entries. */
    int (*labelsetLibrary)[2];      /**< @brief Label set data with columns:
                                         set, labelstring index. */

    int isRfLibraryParsed;          /**< @brief Flag indicating if the RF library was parsed. */
    int rfLibrarySize;              /**< @brief Number of RF entries. */
    float (*rfLibrary)[10];         /**< @brief RF library data with columns:
                                         amp, mag_id, phase_id, time_id, center, delay,
                                         freqPPM, phasePPM, freq, phase. */

    int isRfShimLibraryParsed;      /**< @brief Flag indicating if the RF shim library was parsed. */
    int rfShimLibrarySize;          /**< @brief Number of RF shim entries (channels). */
    float** rfShimLibrary;          /**< @brief RF shim data; per-channel magnitude and phase arrays:
                                         magn_c1, phase_c1, magn_c2, phase_c2, ... */

    int isRotationLibraryParsed;    /**< @brief Flag indicating if the rotation library was parsed. */
    int rotationLibrarySize;        /**< @brief Number of rotation entries. */
    float (*rotationLibrary)[4];    /**< @brief Rotation quaternion data with columns:
                                         RotQuat0, RotQuatX, RotQuatY, RotQuatZ. */

    int isShapeLibraryParsed;       /**< @brief Flag indicating if the shape library was parsed. */
    int shapeLibrarySize;           /**< @brief Number of shape entries. */
    ShapeArbitrary** shapeLibrary;  /**< @brief Array of pointers to arbitrary shape structures. */

    int isSoftDelayLibraryParsed;   /**< @brief Flag indicating if the soft delay library was parsed. */
    int softDelayLibrarySize;       /**< @brief Number of soft delay entries. */
    int (*softDelayLibrary)[3];     /**< @brief Soft delay data with columns:
                                         numID, offset, factor. */
    char (*softDelayHintLibrary)[SOFT_DELAY_HINT_LENGTH]; /**< @brief Hint strings related to soft delays. */

    int isTriggerLibraryParsed;     /**< @brief Flag indicating if the trigger library was parsed. */
    int triggerLibrarySize;         /**< @brief Number of trigger entries. */
    int (*triggerLibrary)[4];       /**< @brief Trigger library data with columns:
                                         duration, delay, type, channel. */
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

void readDefinitions(SeqFile* seq);
void readLibraries(SeqFile* seq, int readBlocks);

// SeqBlock getBlock(SeqFile* seq, int blockIndex);

#endif /* SEQFILE_H */