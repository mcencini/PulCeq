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
/** @struct ShapeArbitrary
   * @brief  Arbitrary shape struct
   *
   *  @var ShapeArbitrary::nSamples
   *    Number of waveform samples.
   *  @var ShapeArbitrary::samples
   *    Array of waveform samples.
   */
typedef struct {
    int nSamples;   /* @brief Number of waveform samples */
    float *samples; /* @brief Waveform samples */
} ShapeArbitrary; /* mirrors Pulseq CompressedShape */

/** @struct ShapeTrap
   * @brief  Trapzoid shape struct
   *
   *  @var ShapeTrap::riseTime
   *    Ramp up time of trapezoid (us).
   *  @var ShapeTrap::flatTime
   *    Flat-top time of trapezoid (us).
   *  @var ShapeTrap::fallTime
   *    Ramp down time of trapezoid (us).
   */
typedef struct {
    long riseTime; /* @brief Ramp up time of trapezoid (us)  */
    long flatTime; /* @brief Flat-top time of trapezoid (us)  */
    long fallTime; /* @brief Ramp down time of trapezoid (us) */
} ShapeTrap; /* no Pulseq equivalent */

/****************************************************************/ 
/*                  Pulseq event structs                        */
/****************************************************************/
/** @struct RFEvent
   * @brief  RF event
   *
   *  @var RFEvent::type
   *    Whether RF is NULL (0) or defined (1).
   *  @var RFEvent::amplitude
   *    Peak magnitude of magShape (Hz).
   *  @var RFEvent::magShape
   *    Magnitude waveform shape.
   *  @var RFEvent::phaseShape
   *    Phase waveform shape (for complex-valued RF pulses).
   *  @var RFEvent::timeShape
   *    Timepoints for RF waveform shape (for irregular raster).
   *  @var RFEvent::center
   *    Effective RF center of the pulse shape measured from the start of the shape (us).
   *  @var RFEvent::freqPPM
   *    B0-dependent frequency offset of transmitter (ppm).
   *  @var RFEvent::phasePPM
   *    B0-dependent phase offset of transmitter (rad/MHz).
   *  @var RFEvent::freqOffset
   *    Frequency offset of transmitter (Hz).
   *  @var RFEvent::phaseOffset
   *    Phase offset of transmitter (rad).
   *  @var RFEvent::delay
   *    Delay prior to the pulse (us).
   * @var RFEvent::use 
   *    Single character indicating the intended use of the pulse, e.g. e,r,etc...
   */
typedef struct {
    short type;                /* @brief NULL or ARBITRARY */    
    float amplitude;           /* @brief Peak magnitude of magShape (Hz) */
    ShapeArbitrary magShape;   /* @brief Arbitrary waveform, unitary peak amplitude */
    ShapeArbitrary phaseShape; /* @brief Abitrary waveform */
    ShapeArbitrary timeShape;  /* @brief Arbitrary waveform */
    float center;              /* @brief Effective RF center of the pulse shape measured from the start of the shape (us) */
    float freqPPM;             /* @brief B0-dependent frequency offset of transmitter (ppm) */
    float phasePPM;            /* @brief B0-dependent phase offset of transmitter (rad/MHz) */
    float freqOffset;          /* @brief Frequency offset of transmitter (Hz) */
	float phaseOffset;         /* @brief Phase offset of transmitter (rad) */
    int delay;                 /* @brief Delay prior to the pulse (us) */
    char use;                  /* @brief Single character indicating the intended use of the pulse, e.g. e,r,etc... */
} RFEvent; /* mirrors Pulseq RFEvent */

/** @struct PulseqGrad
   * @brief  Gradient event. 
   *
   *  @var GradEvent::type
   *    Whether gradient is NULL (0), TRAP (1) or ARBITRARY (2).
   *  @var GradEvent::amplitude
   *    Peak amplitude of the gradient (Hz/m).
   *  @var GradEvent::delay
   *    Delay prior to the gradient (us).
   *  @var GradEvent::trap
   *    Trapezoid shape (for type == 1).
   *  @var GradEvent::waveShape
   *    Gradient waveform shape (for type == 2).
   *  @var GradEvent::timeShape
   *    Timepoints for Gradient waveform shape (for type == 2).
   *  @var GradEvent::first
   *    Amplitude at the start of the shape (for type == 2).
   *  @var GradEvent::last
   *    Amplitude at the end of the shape(for type == 2).
   */
typedef struct {
    short type;               /* @brief NULL, TRAP, or ARBITRARY */  
    float amplitude;          /* @brief Peak amplitude of the gradient (Hz/m) */
    int delay;                /* @brief Delay prior to the gradient (us) */
    ShapeTrap trap;           /* @brief Trapezoid, unitary plateau amplitude */
    ShapeArbitrary waveShape; /* @brief Arbitrary waveform, unitary peak amplitude */
    ShapeArbitrary timeShape; /* @brief Arbitrary waveform */
    float first;              /* @brief Amplitude at the start of the shape for arbitrary gradient */
    float last;               /* @brief Amplitude at the end of the shape for arbitrary gradient */
} GradEvent; /* mirrors Pulseq GradEvent */

/** @struct ADCEvent
   * @brief  ADC event. 
   *
   *  @var ADCEvent::type
   *    Whether ADC is NULL (0) or defined (1).
   *  @var ADCEvent::numSamples
   *    Number of ADC samples.
   *  @var ADCEvent::dwellTime
   *    Dwell time of ADC readout (ns).
   *  @var ADCEvent::delay
   *    Delay before first sample (us).
   *  @var ADCEvent::freqPPM
   *    B0-dependent frequency offset of receiver (ppm).
   *  @var ADCEvent::phasePPM
   *    B0-dependent phase offset of receiver (rad/MHz).
   *  @var ADCEvent::freqOffset
   *    Frequency offset of receiver (Hz).
   *  @var ADCEvent::phaseOffset
   *    Phase offset of receiver (rad).
   *  @var ADCEvent::phaseModulationShape
   *    Phase modulation shape of receiver (rad).
   */
typedef struct {
    short type;                          /* @brief NULL or ADC */
    int numSamples;                      /* @brief Number of ADC samples */
    int dwellTime;                       /* @brief Dwell time of ADC readout (ns) */
    int delay;                           /* @brief Delay before first sample (us) */
    float freqPPM;                       /* @brief B0-dependent frequency offset of receiver (ppm) */
    float phasePPM;                      /* @brief B0-dependent phase offset of receiver (rad/MHz) */
    float freqOffset;                    /* @brief Frequency offset of receiver (Hz) */
	float phaseOffset;                   /* @brief Phase offset of receiver (rad) */
    ShapeArbitrary phaseModulationShape; /* @brief Phase modulation shape of receiver (rad) */
} ADCEvent; /* mirrors Pulseq ADCEvent */

/** @struct TriggerEvent
   * @brief  Trigger event. 
   *
   *  @var TriggerEvent::type
   *    Whether trigger is OFF (0) or ON (1).
   *  @var TriggerEvent::duration
   *    Duration of trigger event (us).
   *  @var TriggerEvent::delay
   *    Delay prior to the trigger event (us).
   *  @var TriggerEvent::triggerType
   *    Type of trigger (system dependent). 0: undefined / unused.
   *  @var TriggerEvent::triggerChannel
   *    Channel of trigger (system dependent). 0: undefined / unused.
   */
typedef struct {
    short type;         /* @brief OFF or ON */
    long duration;      /* @brief Duration of trigger event (us) */
    long delay;         /* @brief Delay prior to the trigger event (us) */
    int triggerType;    /* @brief Type of trigger (system dependent). 0: undefined / unused */
    int triggerChannel; /* @brief Channel of trigger (system dependent). 0: undefined / unused */
} TriggerEvent; /* mirrors Pulseq TriggerEvent */

#define SOFT_DELAY_HINT_LENGTH 32
/** @struct SoftDelayEvent
   * @brief  Soft Delay event. 
   *
   *  @var SoftDelayEvent::type
   *    Whether delay is NULL (0) or DEFINED (1).
   *  @var SoftDelayEvent::numID
   *    Numeric index of the soft delay to help the intepreter (together with the hint string) to identify the delay and allocate it to the UI element.
   *  @var SoftDelayEvent::offset
   *    Offset (positive or negative) added to the delay after the division by the factor (us).
   *  @var SoftDelayEvent::factor
   *    Factor by which the value on the user interface needs to be divided for calculating the final delay applied to the sequence.
   *  @var SoftDelayEvent::hint
   *    Text hint corresponding to this soft delay, e.g. TE.
   */
typedef struct {
    short type;                        /* @brief NULL or DEFINED */
    int numID;                         /* @brief Numeric index of the soft delay to help the intepreter (together with the hint string) to identify the delay and allocate it to the UI element */
    int offset;                        /* @brief Offset (positive or negative) added to the delay after the division by the factor (us) */
    int factor;                        /* @brief Factor by which the value on the user interface needs to be divided for calculating the final delay applied to the sequence */
    char hint[SOFT_DELAY_HINT_LENGTH]; /* @brief Text hint corresponding to this soft delay, e.g. TE */
} SoftDelayEvent; /* mirrors Pulseq SoftDelayEvent */

/** @struct RotationEvent
   * @brief  Rotation event. 
   *
   *  @var RotationEvent::type
   *    Whether rotation is NULL (0) or DEFINED (1).
   *  @var RotationEvent::rotQuaternion
   *    Gradient rotation quaternion.
   */
typedef struct {
    short type;        /* @brief NULL or DEFINED */
    double rotQuaternion; /* @brief Gradient rotation quaternion */
} RotationEvent; /* mirrors Pulseq RotationEvent */

/** @struct RfShimmingEvent
   * @brief  RF Shimming event. 
   *
   *  @var RfShimmingEvent::type
   *    Whether rf shimming is NULL (0) or DEFINED (1).
   *  @var RfShimmingEvent::ID
   *    Unique ID of the RF shimming object.
   *  @var RfShimmingEvent::nChan
   *    Number of RF channels.
   *  @var RfShimmingEvent::amplitudes
   *    Amplitude scaling factor for each channel.
   *  @var RfShimmingEvent::phases
   *    Additional phase for each channel.
   */
typedef struct {
    short type;        /* @brief NULL or DEFINED */
    int ID;            /* @brief Unique ID of the RF shimming object */
    int nChan;         /* @brief Number of RF channels */
    float* amplitudes; /* @brief Amplitude scaling factor for each channel */
    float* phases;     /* @brief Additional phase for each channel */
} RfShimmingEvent; /* mirrors Pulseq SoftDelayEvent */

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
