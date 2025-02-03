#ifndef PULCEQ_H
#define PULCEQ_H

/****************************************************************/ 
/*            Structs defining normalized shapes                */
/****************************************************************/ 
typedef struct {
    int nSamples; /* Number of waveform samples */
    float *samples;
} PulseqShapeArbitrary; /* mirrors Pulseq CompressedShape */
    
typedef struct {
    int riseTime;         /* Ramp up time of trapezoid (us)  */
    int flatTime;         /* Flat-top time of trapezoid (us)  */
    int fallTime;         /* Ramp down time of trapezoid (us) */
} PulseqShapeTrap; /* no Pulseq equivalent */

/****************************************************************/ 
/*                  Pulseq event structs                        */
/****************************************************************/ 
typedef struct {
    /* Header section */
    short type;                         /* NULL or ARBITRARY */
    
    /* Waveforms */
    PulseqShapeArbitrary magShape;   /* arbitrary waveform, normalized amplitude */
    PulseqShapeArbitrary phaseShape; /* arbitrary waveform */
    PulseqShapeArbitrary timeShape;  /* arbitrary waveform */
    int delay;                       /* Delay prior to the pulse (us) */
    
    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;     /* default: 0 */
    int* userInt;
    float nUserFloat; /* default: 0 */
    float* userFloat;

} PulseqRF; /* mirrors Pulseq RFEvent */

typedef struct {
    /* Header section */
    short type;    /* NULL, TRAP, or ARBITRARY */
    
    int delay; /* Delay prior to the gradient (us) */
    
    /* Waveforms */
    PulseqShapeTrap trap;           /* trapezoid, normalized amplitude */
    PulseqShapeArbitrary waveShape; /* arbitrary waveform, normalized amplitude */
    PulseqShapeArbitrary timeShape; /* arbitrary waveform */
    
} PulseqGrad; /* mirrors Pulseq GradEvent */

typedef struct {
    /* Header section */
    short   type; /* NULL or ADC */
    
    int numSamples;  /* Number of ADC samples */
    int dwellTime; /* Dwell time of ADC readout (ns) */
    int delay;     /* Delay before first sample (us) */
    
} PulseqADC; /* mirrors Pulseq ADCEvent */

typedef struct {
    /* Header section */
    short   type; /* Trigger off (type == 0) or on (type == 1) */
    
    int duration;        /* Duration of trigger event (us) */
    int delay;           /* Delay prior to the trigger event (us) */
    int triggerType;     /* Type of trigger (system dependent). 0: undefined / unused */
    int triggerChannel;  /* Channel of trigger (system dependent). 0: undefined / unused */
    
} PulseqTrig; /* mirrors Pulseq TriggerEvent */

/*********************************************************************************************************/ 
/*                                   Block, segment, and sequence structs                                */
/*********************************************************************************************************/ 
/* Block struct - a (typically short) array of these is used to contain a list of the base/parent blocks */
typedef struct {
    /* Header section */
    int ID; /* Unique block ID */

    /* Block definition */
    float      duration; /* sec */
    PulseqRF   rf;
    PulseqGrad gx;
    PulseqGrad gy;
    PulseqGrad gz;
    PulseqADC  adc;
    PulseqTrig trig;

    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;     /* default: 0 */
    int* userInt;
    float nUserFloat; /* default: 0 */
    float* userFloat;
    
} PulseqBlock; /* mirrors Pulseq SeqBlock */

/* Struct containing block IDs that make up a segment */
typedef struct {
    /* Header section */
    short  segmentID; /* Unique segment ID */
    
    /* Segment definition */
    short  nBlocksInSegment;
    short* blockIDs;         /* Block ID's in this segment */
    
    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;     /* default: 0 */
    int* userInt;
    float nUserFloat; /* default: 0 */
    float* userFloat;
    
} Segment; /* no Pulseq equivalence */

/* Struct containing entire sequence definition */
typedef struct {
    /* Header section */
    short version_major;          
	short version_minor;
	short version_revision;
	short version_combined;
	
	/* Base Pulseq blocks */
    short nParentBlocks;          
    PulseqBlock* parentBlocks;

    /* Sequence segments; optional */ 
    short nSegments;             
    Segment* segments;

    /* Dynamic scan settings */
    int nRowsInLoopArray;       /* Number of rows (length of BLOCKS section in .seq file) */
    short nColumnsInLoopArray;  /* Number of columns */
    float** loop                

    /*********************************************************/
    /*                     Loop definition                   */
    /*********************************************************/
    /* # Column | Name          | Units           | Notes    */
    /* ------------------------------------------------------*/
    /*        0 | segmentID     | int             |          */
    /*        1 | blockID       | int             |          */
    /*        2 | rfamp         | Hz              |          */
    /*        3 | rfphs         | rad             |          */
    /*        4 | rffreq        | Hz              |          */
    /*        5 | gxamp         | Hz/m            |          */
    /*        6 | gxenergy      | (Hz/m)**2 * sec |          */
    /*        7 | gyamp         | Hz/m            |          */
    /*        8 | gyenergy      | (Hz/m)**2 * sec |          */
    /*        9 | gzamp         | Hz/m            |          */
    /*       10 | gzenergy      | (Hz/m)**2 * sec |          */
    /*       11 | recphs        | rad             |          */
    /*       12 | blockDuration | sec             |          */
    /*       13 | physioTrigger | short           |          */
    /*       14 | rotmat[0][0]  | float           | optional */
    /*       15 | rotmat[0][1]  | float           | optional */
    /*       16 | rotmat[0][2]  | float           | optional */
    /*       17 | rotmat[1][0]  | float           | optional */
    /*       18 | rotmat[1][1]  | float           | optional */
    /*       19 | rotmat[1][2]  | float           | optional */
    /*       20 | rotmat[2][0]  | float           | optional */
    /*       21 | rotmat[2][1]  | float           | optional */
    /*       22 | rotmat[2][2]  | float           | optional */
    /*********************************************************/
    
    /* Raster times (sec) */
	float adc_raster_us;               /* Siemens default: 0.1us; GE default: 2us */
	float grad_raster_us;              /* Siemens default: 10us; GE default: 4us */
	float rf_raster_us;                /* Siemens default: 1us; GE default: 2us (?) */
	/* float block_duration_raster_us; */ /* Siemens default: 10uss; GE default: 2us */
        
    /* User parameters arrays available for use as needed by the client program */
    /* Must be defined to be allocated dynamically by the client program */
    int nUserInt;     /* default: 0 */
    int* userInt;
    float nUserFloat; /* default: 0 */
    float* userFloat;
   
} SegmentedSequence; /* mirrors Pulseq ExternalSequence */

/* function prototypes that this specification implements */
void read_seq_frombuffer(SegmentedSequence* seq, FILE* fid, int byteswap);
void read_seq_fromfile(SegmentedSequence* seq, const char* filename, int byteswap);

#endif
