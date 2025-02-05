#ifndef PULSEG_H
#define PULSEG_H

/****************************************************************/ 
/*            Structs defining normalized shapes                */
/****************************************************************/ 
typedef struct {
    int nSamples; /* Number of waveform samples */
    float *samples;
} PulseqShapeArbitrary; /* mirrors Pulseq CompressedShape */
    
typedef struct {
    int riseTime; /* Ramp up time of trapezoid (us)  */
    int flatTime; /* Flat-top time of trapezoid (us)  */
    int fallTime; /* Ramp down time of trapezoid (us) */
} PulseqShapeTrap; /* no Pulseq equivalent */

/****************************************************************/ 
/*                  Pulseq event structs                        */
/****************************************************************/ 
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

typedef struct {
    /* Header section */
    short type; /* NULL, TRAP, or ARBITRARY */
    
    int delay;  /* Delay prior to the gradient (us) */
    
    /* Waveforms */
    PulseqShapeTrap trap;           /* Trapezoid, normalized amplitude */
    PulseqShapeArbitrary waveShape; /* Arbitrary waveform, normalized amplitude */
    PulseqShapeArbitrary timeShape; /* Arbitrary waveform */
    
} PulseqGrad; /* mirrors Pulseq GradEvent */

typedef struct {
    /* Header section */
    short   type; /* NULL or ADC */
    
    int numSamples; /* Number of ADC samples */
    int dwellTime;  /* Dwell time of ADC readout (ns) */
    int delay;      /* Delay before first sample (us) */
    
} PulseqADC; /* mirrors Pulseq ADCEvent */

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
/* Block struct - a (typically short) array of these is used to contain a list of the base/parent blocks */
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

/* Struct containing block IDs that make up a segment */
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

/* Struct containing dynamic scan settings */
typedef struct {
    /* Header section */
    int nRowsInLoopArray;       /* Number of rows (length of BLOCKS section in .seq file) */
    short nColumnsInLoopArray;  /* Number of columns */

    short *columnIdx            /* Sparse column index */
    float** values              /* Dynamic scan settings */

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

/* Struct containing entire sequence definition */
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
    Loop** loop                /* Dynamic scan settings */
    
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

/* function prototypes that this specification implements */
void read_seq_frombuffer(SegmentedSequence* seq, FILE* fid, int byteswap);
void read_seq_fromfile(SegmentedSequence* seq, const char* filename);

#endif
