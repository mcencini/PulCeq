#ifndef SEQUENCE_H
#define SEQUENCE_H

/**
 * @struct Segment
 * @brief Represents a sequence segment (collection of blocks/instructions).
 *
 * Contains block IDs, waveform instructions, and flags controlling
 * FOV rotation, position shift, and scaling, as well as prospective motion correction.
 */
typedef struct {
    int numBlocks;         /**< Number of blocks in this segment */
    int *blockIDs;         /**< Array of block IDs defining this segment */
    int pmc;               /**< Whether segment can be prospectively motion corrected or not */
    int norot;             /**< Ignore rotation flag */
    int nopos;             /**< Ignore position shift flag */
    int noscl;             /**< Ignore FOV scaling flag */
} Segment;

/**
 * @struct TR
 * @brief Represents a repetition time (TR) structure.
 *
 * Contains ordered segment indices, cursor for scan loop, execution history,
 * once flag, and duration.
 * 
 */
typedef struct {
    float duration;        /**< Duration of this TR */
    int numSegments;       /**< Number of segments in this TR */
    int *segmentIdx;       /**< Ordered array of segment indices composing this TR */
    int cursor;            /**< Current position within TR (for scan loop) */
    int history;           /**< How many times this TR has been executed */
    int onceFlag;          /**< TR once flag (0, 1, 2) */
} TR;

/**
 * @struct ScanLoop
 * @brief Represents the scan loop structure containing dynamic waveform parameters.
 *
 * Contains segment ID, number of blocks, and arrays for user-defined parameters.
 */
typedef struct {
    int (*eventTable)[3];           /**< @brief Event table with numBlocks rows and columns: trID rfID gradID adcID */
    float (*rfTable)[3];            /**< @brief RF table with numRF rows and columns: rfAmp, rfPhase, rfFreq */
    float (*gradTable)[4];          /**< @brief Gradient table with numGrad rows and columns: gxAmp, gyAmp, gzAmp, rotID */
    float (*adcTable)[3];           /**< @brief ADC table with numReadouts rows and columns: adcPhase, adcFreq, adcLabel */
    float (*rotMatrixTable)[9];     /**< @brief Rotation matrix table with numRotations rows and columns: m11, m12, m13, m21, m22, m23, m31, m32, m33 */
    float (*rotQuaternionTable)[4]; /**< @brief Rotation quaternion table with numRotations rows and columns: q0, q1, q2, q3 */
    int** labelTable;               /**< @brief Label table with numReadouts rows and numLabels[vendor] columns */
} ScanLoop;

/**
 * @struct Sequence
 * @brief Top-level structure representing a parsed Pulseq sequence.
 *
 * Contains all segments, TRs, and global sequence parameters such as
 * maximum RF amplitude, gradient amplitude, slew rate, and total duration.
 */
typedef struct {
    float duration;        /**< Total sequence duration */
    int numReadouts;       /**< Number of readouts in the sequence */
    int* maxLabel;         /**< Max value for each label (data axis) */

    float maxRF;           /**< Maximum RF amplitude in sequence */
    float maxGrad;         /**< Maximum gradient amplitude in sequence */
    float maxSlew;         /**< Maximum slew rate in sequence */

    int numSegments;       /**< Number of segments */
    Segment *segments;     /**< Array of all segments */

    int numTRs;            /**< Number of TRs */
    TR *trs;               /**< Array of all TRs */

    ScanLoop loop;         /**< Scan loop structure containing dynamic waveform parameters */

} Sequence;

#endif /* SEQUENCE_H */