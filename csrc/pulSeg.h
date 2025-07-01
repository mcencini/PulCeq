/**
 * @file pulSeg.h
 */
#ifndef PULSEG_H
#define PULSEG_H

/** 
   * Default ALLOC to malloc if it's not already defined
   *
   * Users are encouraged to replace with vendor-specific implementations if needed:
   * 
   * // pulSeg_vendor.h (vendor-specific header)
   * 
   * #include "my_vendor_library.h"  // This contains the definition of MyVendorAlloc
   * 
   * // Override ALLOC to use MyVendorAlloc in the vendor environment
   * #define ALLOC(size) MyVendorAlloc(size)  // Replaces malloc with MyVendorAlloc
   * 
   * #include "pulSeg.h"  // Now include the vendor-agnostic pulSeg.h with the overridden ALLOC
   * 
   * // Other vendor-specific declarations can go here
   */
#ifndef ALLOC
    #define ALLOC(size) malloc(size)
#endif

#ifndef FREE
  #define FREE(ptr) free(ptr)
#endif

/*********************************************************************************************************/ 
/*                             Block, Segment(s), Loop, and Sequence structs                             */
/*********************************************************************************************************/ 
/** @struct SeqBlock
   * @brief  Block struct - a (typically short) array of these is used to contain a list of the base/parent blocks
   *
   *  @var SeqBlock::ID
   *    Unique block ID.
   *  @var SeqBlock::duration_ru
   *    Block duration in block raster units.
   *  @var SeqBlock::blockDurationRaster
   *    Duration raster (us).
   *  @var SeqBlock::rf
   *    RF event in the block.
   *  @var SeqBlock::gx
   *    X-axis Grad event in the block along X-axis.
   *  @var SeqBlock::gy
   *    Y-axis Grad event in the block along Y-axis.
   *  @var SeqBlock::gz
   *    Z-axis Grad event in the block along Z-axis.
   *  @var SeqBlock::adc
   *    ADC event in the block.
   *  @var SeqBlock::trig
   *    Trigger event in the block.
   *  @var SeqBlock::softDelay
   *    SoftDelay event in the block.
   *  @var SeqBlock::rotation
   *    Rotation event in the block.
   *  @var SeqBlock::rfShim
   *    RF Shimming event in the block.
   */
typedef struct {
    int ID;                     /* @brief Unique block ID */
    long duration_ru;           /* @brief Duration of the block in raster units */
    double blockDurationRaster; /* @brief Duration raster (us) */
    RFEvent   rf;               /* @brief RF event in the block */
    GradEvent gx;               /* @brief Grad event in the block along X-axis */
    GradEvent gy;               /* @brief Grad event in the block along Y-axis */
    GradEvent gz;               /* @brief Grad event in the block along Z-axis */
    ADCEvent  adc;              /* @brief ADC event in the block */
    TriggerEvent trig;          /* @brief Trigger event in the block */
    SoftDelayEvent softDelay;   /* @brief SoftDelay event in the block */
    RotationEvent rotation;     /* @brief Rotation event in the block */
    RfShimmingEvent rfShim;     /* @brief RF Shimming event in the block */
} SeqBlock; /* mirrors Pulseq SeqBlock */

/** @struct Segment
   * @brief  Struct containing block IDs that make up a segment 
   *
   *  @var Segment::segmentID
   *    Unique segment ID.
   *  @var Segment::nBlocksInSegment
   *    Number of Blocks in this segment.
   *  @var Segment::blockIDs
   *    Block ID's in this segment.
   *  @var Segment::nUserInt
   *    Number of integer user parameters.
   *  @var Segment::nUserInt
   *    Array of integer user parameters.
   *  @var Segment::nUserFloat
   *    Number of floating point user parameters.
   *  @var Segment::userFloat
   *    Array of floating point user parameters.
   */
typedef struct {
    short segmentID; /* Unique segment ID */
    
    /* Segment definition */
    short nBlocksInSegment;
    short* blockIDs;         /* Block ID's in this segment */
    
    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;     /* Default: 0 */
    int* userInt;
    int nUserFloat; /* Default: 0 */
    float* userFloat;
    
} Segment; /* no Pulseq equivalence */

/**
 * @struct Loop
 * @brief Structure representing a set of parameters and flags for loop-based operations, typically used in MRI sequence definitions.
 * 
 * This struct contains various parameters and flags related to the settings for each block in a loop, such as RF amplitude, frequency, phase, gradient amplitudes, and others. These parameters control the behavior of MRI sequence blocks, with flags indicating whether each parameter is constant or variable over time. Additionally, the struct provides support for user-defined fields and various flags that control specific sequence behaviors.
 * 
 * The struct also includes definitions for loop columns and their units, detailing how data is organized and interpreted in the loop.
 * 
 * @note The struct's memory management and usage involve dynamic arrays for parameters that change over time, such as RF and gradient amplitudes. The user is responsible for allocating and freeing memory as needed for the dynamic fields.
 * 
 * @var Loop::nBlocks
 *   Number of rows (length of BLOCKS section in .seq file).
 * 
 * @var Loop::segmentID
 *   Array of segment IDs, one per block.
 * 
 * @var Loop::blockID
 *   Array of block IDs, one per block.
 * 
 * @var Loop::rfAmpFlag
 *   Flag indicating whether the RF amplitude is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::rfAmp
 *   Array of RF amplitude values (in Hz) for each block, if variable.
 * 
 * @var Loop::rfPhsFlag
 *   Flag indicating whether the RF phase is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::rfPhs
 *   Array of RF phase values (in radians) for each block, if variable.
 * 
 * @var Loop::rfFreqFlag
 *   Flag indicating whether the RF frequency is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::rfFreq
 *   Array of RF frequency values (in Hz) for each block, if variable.
 * 
 * @var Loop::gxAmpFlag
 *   Flag indicating whether the x-gradient amplitude is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::gxAmp
 *   Array of x-gradient amplitudes (in Hz/m) for each block, if variable.
 * 
 * @var Loop::gyAmpFlag
 *   Flag indicating whether the y-gradient amplitude is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::gyAmp
 *   Array of y-gradient amplitudes (in Hz/m) for each block, if variable.
 * 
 * @var Loop::gzAmpFlag
 *   Flag indicating whether the z-gradient amplitude is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::gzAmp
 *   Array of z-gradient amplitudes (in Hz/m) for each block, if variable.
 * 
 * @var Loop::recPhsFlag
 *   Flag indicating whether the receiver phase is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::recPhs
 *   Array of receiver phase values (in radians) for each block, if variable.
 * 
 * @var Loop::blockDuration
 *   Array of block durations (in seconds) for each block.
 * 
 * @var Loop::physioTrigFlag
 *   Flag indicating whether the physiological trigger is constant (0) or variable (1) across blocks.
 * 
 * @var Loop::physioTrig
 *   Array of physiological trigger flags (0: OFF, 1: ON) for each block, if variable.
 * 
 * @var Loop::rotangleFlag
 *   Flag indicating whether the in-plane rotation angle is constant (0) or variable (1) across blocks.
 *   User can specify either this or 3D rotation matrix.
 * 
 * @var Loop::rotangle
 *   Array of in-plane rotation angles (in radians) for each block, if variable.
 * 
 * @var Loop::rotmatFlag
 *   Flag indicating whether the rotation matrix is NULL (0) or defined (1) across blocks.
 * 
 * @var Loop::rotmat
 *   Array of rotation matrices (3x3) for each block, if variable. Each matrix contains 9 values representing a 3D rotation.
 *   User can specify either this or in-plane rotation angle.
 * 
 * @var Loop::PMCFlag
 *   Flag indicating whether the PMC (Prospective Motion Corrention) is used (1) or not (0).
 * 
 * @var Loop::PMC
 *   Array of PMC flags for each block.
 * 
 * @var Loop::NOROTFlag
 *   Flag indicating whether FOV rotation is disabled (1) or enabled (0) for each block.
 * 
 * @var Loop::NOROT
 *   Instruct the interpreter to ignore FOV rotation for a given block.
 * 
 * @var Loop::NOPOSFlag
 *   Flag indicating whether FOV translation is disabled (1) or enabled (0) for each block.
 * 
 * @var Loop::NPOS
 *   Instruct the interpreter to ignore FOV translation for a given block.
 * 
 * @var Loop::NOSLCFlag
 *   Flag indicating whether FOV scaling is disabled (1) or enabled (0) for each block.
 * 
 * @var Loop::NOSLC
 *   Instruct the interpreter to ignore FOV scaling for a given block.
 * 
 * @var Loop::user1Flag to user9Flag
 *   Flags indicating whether user-defined parameters (user1 to user9) are NULL (0) or defined (1).
 * 
 * @var Loop::user1Flag to user9Flag
 *   Arrays of user-defined parameters for each block, if variable.
 * 
 * @note: The user parameters can be used to store vendor-specific parameters for different tasks.
 * 
 *  For example, GE interpreter reserve user1-3 to Gx,y,z energies in (Hz / m)**2 * sec to 
 *  inform proprietary gradient safety checks.
 */
typedef struct {
    int nBlocks;         /* Number of rows (length of BLOCKS section in .seq file) */

    short* segmentID;
    short* blockID;

    short rfAmpFlag;     /* 0: constant; 1: variable */
    float* rfAmp;        /* Hz */

    short rfPhsFlag;     /* 0: constant; 1: variable */
    float* rfPhs;        /* rad */

    short rfFreqFlag;     /* 0: constant; 1: variable */
    float* rfFreq;        /* Hz */

    short gxAmpFlag;      /* 0: constant; 1: variable */
    float* gxAmp;         /* Hz / m */

    short gyAmpFlag;      /* 0: constant; 1: variable */
    float* gyAmp;         /* Hz / m */

    short gzAmpFlag;      /* 0: constant; 1: variable */
    float* gzAmp;         /* Hz / m */

    short recPhsFlag;     /* 0: constant; 1: variable */
    float* recPhs;        /* rad */

    float* blockDuration; /* sec */

    short physioTrigFlag; /* 0: constant; 1: variable */
    short* physioTrig;    /* 0: OFF; 1: ON */

    short rotangleFlag;   /* 0: constant; 1: variable */
    float* rotangle;      /* rad */

    short rotmatFlag;     /* 0: constant; 1: variable */
    float (*rotmat)[9];

    /* Counters to inform reconstruction */
    short SLCFlag;
    int* SLC; /* slice counter (or slab counter for 3D multi-slab sequences) */

    short SEGlag;
    int* SEG; /* segment counter e.g. for segmented FLASH or EPI */

    short REPFlag;
    int* REP; /* repetition counter */

    short AVGFlag;
    int* AVG; /*averaging counter */

    short SETFlag;
    int* SET; /* flexible counter without firm assignment */

    short ECOFlag;
    int* ECO; /* echo counter in multi-echo sequences */

    short PHSFlag;
    int* PHS; /* cardiac phase counter */

    short LINFlag;
    int* LIN; /* line counter in 2D and 3D acquisitions */

    short PARFlag;
    int* PAR; /* partition counter; it counts phase encoding steps in the 2nd (through-slab) phase encoding direction in 3D sequences */

    short ACQFlag;
    int* ACQ; /* a 3-state flag that instructs the interpreter to alter the sequence when executing multiple repeats as follows: 
                blocks with ONCE==0 are executed on every repetition; ONCE==1: only on the first repetition; ONCE==2: only on the last repetition */

    /* Counters to inform the interpreter */
    short ONCEFlag;
    int* ONCE; /* spectroscopic acquisition counter */

    /* Flags to inform the reconstruction */
    short NAVFlag;
    short* NAV; /* navigator data flag */

    short REVFlag;
    short* REV; /* flag indicating that the readout direction is reversed */

    short SMSFlag;
    short* SMS; /* simultaneous multi-slice (SMS) acquisition */

    short REFFlag;
    short* REF; /* 	parallel imaging flag indicating reference / auto-calibration data */

    short IMAFlag;
    short* IMA; /* parallel imaging flag indicating imaging data within the ACS region */

    short NOISEFlag;
    short* NOISE; /* flag for the noise adjust scan e.g for the parallel imaging acceleration */

    /* Flags to inform the interpreter */
    short PMCFlag;
    short* PMC; /* flag for the MoCo/PMC Pulseq version marking blocks that can/should be prospectively corrected for motion */

    short NOROTFlag;
    short* NOROT; /* instructs the interpreter to ignore the rotation of the FOV specified on the UI for the given block(s) */

    short NOPOSFlag;
    short* NOPOS; /* instructs the interpreter to ignore the the FOV offset specified on the UI for the given block(s) */

    short NOSLCFlag; 
    short* NOSCL; /* instructs the interpreter to ignore the scaling of the FOV specified on the UI for the given block(s) */

} Loop; /* no Pulseq equivalence */

/** @struct SegmentedSequence
   * @brief  Struct containing entire sequence definition 
   *
   *  @var SegmentedSequence::version_combined
   *    Pulseq version used to design the sequence.
   *  @var SegmentedSequence::nParentBlocks
   *    Number of Parent Blocks.
   *  @var SegmentedSequence::parentBlocks
   *    Array of Parent Blocks.
   *  @var SegmentedSequence::nSegments
   *    Number of Segments.
   *  @var SegmentedSequence::segments
   *    Array of Segments.
   *  @var SegmentedSequence::loop
   *    Dynamic scan settings.
   *  @var SegmentedSequence::adc_raster_us
   *    ADC raster times (us).
   *  @var SegmentedSequence::grad_raster_us
   *    Gradient raster times (us).
   *  @var SegmentedSequence::rf_raster_us
   *    Radiofrequency raster times (us).
   *  @var SegmentedSequence::block_duration_raster_us
   *    Block raster times (us).
   *  @var SegmentedSequence::nUserInt
   *    Number of integer user parameters.
   *  @var SegmentedSequence::nUserInt
   *    Array of integer user parameters.
   *  @var SegmentedSequence::nUserFloat
   *    Number of floating point user parameters.
   *  @var SegmentedSequence::userFloat
   *    Array of floating point user parameters.
   */
typedef struct {
	int version_combined; /* 1000000 * version_major + 1000 * version_minor + version_revision */
	
	/* Base Pulseq blocks */
    short nParentBlocks;          
    PulseqBlock* parentBlocks;

    /* Sequence segments; optional */ 
    int nSegments;             
    Segment* segments;

    /* Dynamic scan settings */
    Loop loop;
    
    /* Raster times (sec) */
	float adc_raster_us;            /* Siemens default: 0.1us; GE default: 2us */
	float grad_raster_us;           /* Siemens default: 10us; GE default: 4us */
	float rf_raster_us;             /* Siemens default: 1us; GE default: 2us */
	float block_duration_raster_us; /* Siemens default: 10us; GE default: 4us */
        
    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;   /* Default: 0 */
    int* userInt;
    int nUserFloat; /* Default: 0 */
    float* userFloat;
   
} SegmentedSequence; /* mirrors Pulseq ExternalSequence */

/* TODO: remove when we have tests for reading */
short byteswap_short(short value);
int  byteswap_int(int value);
float byteswap_float(float value);

/* function prototypes that this specification implements */
void read_seq_frombuffer(SegmentedSequence* seq, FILE* fid);
void read_seq_fromfile(SegmentedSequence* seq, const char* filename);

#endif
