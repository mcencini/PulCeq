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

/****************************************************************/ 
/*            Structs defining normalized shapes                */
/****************************************************************/ 
/** @struct PulseqShapeArbitrary
   * @brief  Arbitrary shape struct
   *
   *  @var PulseqShapeArbitrary::nSamples
   *    Number of waveform samples.
   *  @var PulseqShapeArbitrary::samples
   *    Array of waveform samples.
   */
typedef struct {
    int nSamples; /* Number of waveform samples */
    float *samples;
} PulseqShapeArbitrary; /* mirrors Pulseq CompressedShape */

/** @struct PulseqShapeTrap
   * @brief  Trapzoid shape struct
   *
   *  @var PulseqShapeTrap::riseTime
   *    Ramp up time of trapezoid (us).
   *  @var PulseqShapeTrap::flatTime
   *    Flat-top time of trapezoid (us).
   *  @var PulseqShapeTrap::fallTime
   *    Ramp down time of trapezoid (us).
   */
typedef struct {
    int riseTime; /* Ramp up time of trapezoid (us)  */
    int flatTime; /* Flat-top time of trapezoid (us)  */
    int fallTime; /* Ramp down time of trapezoid (us) */
} PulseqShapeTrap; /* no Pulseq equivalent */

/****************************************************************/ 
/*                  Pulseq event structs                        */
/****************************************************************/
/** @struct PulseqRF
   * @brief  RF event
   *
   *  @var PulseqRF::type
   *    Whether RF is NULL (0) or defined (1).
   *  @var PulseqRF::amplitude
   *    Peak magnitude of magShape (Hz).
   *  @var PulseqRF::magShape
   *    Magnitude waveform shape.
   *  @var PulseqRF::phaseShape
   *    Phase waveform shape (for complex-valued RF pulses).
   *  @var PulseqRF::timeShape
   *    Timepoints for RF waveform shape (for irregular raster).
   *  @var PulseqRF::freqOffset
   *    Frequency offset of transmitter (Hz).
   *  @var PulseqRF::phaseOffset
   *    Phase offset of transmitter (rad).
   *  @var PulseqRF::delay
   *    Delay prior to the pulse (us).
   *  @var PulseqRF::nUserInt
   *    Number of integer user parameters.
   *  @var PulseqRF::nUserInt
   *    Array of integer user parameters.
   *  @var PulseqRF::nUserFloat
   *    Number of floating point user parameters.
   *  @var PulseqRF::userFloat
   *    Array of floating point user parameters.
   */
typedef struct {
    /* Header section */
    short type; /* NULL or ARBITRARY */
    
    /* Waveforms */
    float amplitude;                 /* Peak magnitude of magShape (Hz) */
    PulseqShapeArbitrary magShape;   /* Arbitrary waveform, unitary peak amplitude */
    PulseqShapeArbitrary phaseShape; /* Abitrary waveform */
    PulseqShapeArbitrary timeShape;  /* Arbitrary waveform */
    float freqOffset;                /* Frequency offset of transmitter (Hz) */
	float phaseOffset;               /* Phase offset of transmitter (rad) */
    int delay;                       /* Delay prior to the pulse (us) */
    
    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;     /* Default: 0 */
    int* userInt;
    int nUserFloat; /* Default: 0 */
    float* userFloat;

} PulseqRF; /* mirrors Pulseq RFEvent */

/** @struct PulseqGrad
   * @brief  Gradient event. 
   *
   *  @var PulseqGrad::type
   *    Whether gradient is NULL (0), TRAP (1) or ARBITRARY (2).
   *  @var ::amplitude
   *    Peak amplitude of the gradient (Hz/m).
   *  @var PulseqGrad::delay
   *    Delay prior to the gradient (us).
   *  @var PulseqGrad::trap
   *    Trapezoid shape (for type == 1).
   *  @var PulseqGrad::waveShape
   *    Gradient waveform shape (for type == 2).
   *  @var PulseqGrad::timeShape
   *    Timepoints for Gradient waveform shape (for type == 2).
   */
typedef struct {
    /* Header section */
    short type; /* NULL, TRAP, or ARBITRARY */
    
    float amplitude; /* Peak amplitude of the gradient (Hz/m) */
    int delay;       /* Delay prior to the gradient (us) */
    
    /* Waveforms */
    PulseqShapeTrap trap;           /* Trapezoid, unitary plateau amplitude */
    PulseqShapeArbitrary waveShape; /* Arbitrary waveform, unitary peak amplitude */
    PulseqShapeArbitrary timeShape; /* Arbitrary waveform */
    
} PulseqGrad; /* mirrors Pulseq GradEvent */

/** @struct PulseqADC
   * @brief  ADC event. 
   *
   *  @var PulseqADC::type
   *    Whether ADC is NULL (0) or defined (1).
   *  @var PulseqADC::numSamples
   *    Number of ADC samples.
   *  @var PulseqADC::dwellTime
   *    Dwell time of ADC readout (ns).
   *  @var PulseqADC::delay
   *    Delay before first sample (us).
   */
typedef struct {
    /* Header section */
    short   type; /* NULL or ADC */
    
    int numSamples; /* Number of ADC samples */
    int dwellTime;  /* Dwell time of ADC readout (ns) */
    int delay;      /* Delay before first sample (us) */
    
} PulseqADC; /* mirrors Pulseq ADCEvent */

/** @struct PulseqTrig
   * @brief  Trigger event. 
   *
   *  @var PulseqTrig::type
   *    Whether trigger is OFF (0) or ON (1).
   *  @var PulseqTrig::duration
   *    Duration of trigger event (us).
   *  @var PulseqTrig::delay
   *    Delay prior to the trigger event (us).
   *  @var PulseqTrig::triggerType
   *    Type of trigger (system dependent). 0: undefined / unused.
   *  @var PulseqTrig::triggerChannel
   *    Channel of trigger (system dependent). 0: undefined / unused.
   */
typedef struct {
    /* Header section */
    short  type; /* OFF or ON */
    
    int duration;        /* Duration of trigger event (us) */
    int delay;           /* Delay prior to the trigger event (us) */
    int triggerType;     /* Type of trigger (system dependent). 0: undefined / unused */
    int triggerChannel;  /* Channel of trigger (system dependent). 0: undefined / unused */
    
} PulseqTrig; /* mirrors Pulseq TriggerEvent */

/*********************************************************************************************************/ 
/*                             Block, Segment(s), Loop, and Sequence structs                             */
/*********************************************************************************************************/ 
/** @struct PulseqBlock
   * @brief  Block struct - a (typically short) array of these is used to contain a list of the base/parent blocks
   *
   *  @var PulseqBlock::ID
   *    Unique block ID.
   *  @var PulseqBlock::duration_ru
   *    Block duration in block raster units.
   *  @var PulseqBlock::rf
   *    RF event in the block.
   *  @var PulseqBlock::gx
   *    X-axis Grad event in the block.
   *  @var PulseqBlock::gy
   *    Y-axis Grad event in the block.
   *  @var PulseqBlock::gz
   *    Z-axis Grad event in the block.
   *  @var PulseqBlock::adc
   *    ADC event in the block.
   *  @var PulseqBlock::trig
   *    Trigger event in the block.
   *  @var PulseqBlock::nUserInt
   *    Number of integer user parameters.
   *  @var PulseqBlock::nUserInt
   *    Array of integer user parameters.
   *  @var PulseqBlock::nUserFloat
   *    Number of floating point user parameters.
   *  @var PulseqBlock::userFloat
   *    Array of floating point user parameters.
   */
typedef struct {
    /* Header section */
    int ID; /* Unique block ID */

    /* Block definition */
    int duration_ru; /* Duration of the block in raster units */
    PulseqRF   rf;
    PulseqGrad gx;
    PulseqGrad gy;
    PulseqGrad gz;
    PulseqADC  adc;
    PulseqTrig trig;

    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;     /* Default: 0 */
    int* userInt;
    int nUserFloat; /* Default: 0 */
    float* userFloat;
    
} PulseqBlock; /* mirrors Pulseq SeqBlock */

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
    /* Header section */
    short  segmentID; /* Unique segment ID */
    
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
 * @var Loop::pmcFlag
 *   Flag indicating whether the PMC (Prospective Motion Corrention) is used (1) or not (0).
 * 
 * @var Loop::pmc
 *   Array of PMC flags for each block.
 * 
 * @var Loop::noRotFlag
 *   Flag indicating whether FOV rotation is disabled (1) or enabled (0) for each block.
 * 
 * @var Loop::noRot
 *   Instruct the interpreter to ignore FOV rotation for a given block.
 * 
 * @var Loop::noPosFlag
 *   Flag indicating whether FOV translation is disabled (1) or enabled (0) for each block.
 * 
 * @var Loop::noPos
 *   Instruct the interpreter to ignore FOV translation for a given block.
 * 
 * @var Loop::noSlcFlag
 *   Flag indicating whether FOV scaling is disabled (1) or enabled (0) for each block.
 * 
 * @var Loop::noSlc
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
    /* Header section */
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

    /* Flags to inform the interpreter */
    short pmcFlag;
    short* pmc;

    short noRotFlag;
    short* noRot;

    short noPosFlag;
    short* noPos;

    short noSlcFlag;
    short* noSlc;

    /* USER */
    short user1Flag;
    float* user1;
    short user2Flag;
    float* user2;
    short user3Flag;
    float* user3;
    short user4Flag;
    float* user4;
    short user5Flag;
    float* user5;
    short user6Flag;
    float* user6;
    short user7Flag;
    float* user7;
    short user8Flag;
    float* user8;
    short user9Flag;
    float* user9;

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
    /* Header section */
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
    int nUserInt;     /* Default: 0 */
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
