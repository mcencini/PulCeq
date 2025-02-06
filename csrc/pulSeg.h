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
   *  @var PulseqRF::magShape
   *    Magnitude waveform shape.
   *  @var PulseqRF::phaseShape
   *    Phase waveform shape (for complex-valued RF pulses).
   *  @var PulseqRF::timeShape
   *    Timepoints for RF waveform shape (for irregular raster).
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
    PulseqShapeArbitrary magShape;   /* Arbitrary waveform, normalized amplitude */
    PulseqShapeArbitrary phaseShape; /* Abitrary waveform */
    PulseqShapeArbitrary timeShape;  /* Arbitrary waveform */
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
    
    int delay;  /* Delay prior to the gradient (us) */
    
    /* Waveforms */
    PulseqShapeTrap trap;           /* Trapezoid, normalized amplitude */
    PulseqShapeArbitrary waveShape; /* Arbitrary waveform, normalized amplitude */
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

/** @struct Loop
   * @brief  Struct containing dynamic scan settings 
   *
   *  @var Loop::nRowsInLoopArray
   *    Number of rows (blocks) in the loop structure.
   *  @var Loop::nColumnsInLoopArray
   *    Number of columns (parameters) in the loop structure.
   *  @var Loop::columnIdx
   *    Sparse loop column indexes.
   *  @var Loop::values
   *    Sparse loop values.
   */
typedef struct {
    /* Header section */
    int nRowsInLoopArray;       /* Number of rows (length of BLOCKS section in .seq file) */
    short nColumnsInLoopArray;  /* Number of columns */

    short *columnIdx;           /* Sparse column index */
    float** values;             /* Dynamic scan settings */

    /**********************************************/
    /*              Loop definition               */
    /**********************************************/
    /* # Column | Name          | Units           */
    /* -------------------------------------------*/
    /*        0 | segmentID     | int             */
    /*        1 | blockID       | int             */
    /*        2 | rfamp         | Hz              */
    /*        3 | rfphs         | rad             */
    /*        4 | rffreq        | Hz              */
    /*        5 | gxamp         | Hz/m            */
    /*        6 | gyamp         | Hz/m            */
    /*        7 | gzamp         | Hz/m            */
    /*        8 | recphs        | rad             */
    /*        9 | blockDuration | sec             */
    /*       10 | physioTrigger | short           */
    /*       11 | rotangle      | rad             */
    /*       12 | rotmat[0][0]  | n.a.            */
    /*       13 | rotmat[0][1]  | n.a.            */
    /*       14 | rotmat[0][2]  | n.a.            */
    /*       15 | rotmat[1][0]  | n.a.            */
    /*       16 | rotmat[1][1]  | n.a.            */
    /*       17 | rotmat[1][2]  | n.a.            */
    /*       18 | rotmat[2][0]  | n.a.            */
    /*       19 | rotmat[2][1]  | n.a.            */
    /*       20 | rotmat[2][2]  | n.a.            */
    /*       21 | gxenergy      | (Hz/m)**2 * sec */
    /*       22 | gyenergy      | (Hz/m)**2 * sec */
    /*       23 | gzenergy      | (Hz/m)**2 * sec */
    /**********************************************/

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
