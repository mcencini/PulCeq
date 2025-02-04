#include <string.h>
#include <stdint.h>
#include <math.h>

#include "sdkver.h" 
 
#include "pulSeg.h"


/* Byteswap routines */
/* Custom byte-swap function for a 16-bit value (short) */
short byteswap_short(short value) {
    return (short)(((value >> 8) & 0x00FF) | 
                   ((value << 8) & 0xFF00));
}

/* Custom byte-swap function for a 32-bit value (int) */
int byteswap_int(int value) {
    return ((value >> 24) & 0x000000FF) | 
           ((value >> 8)  & 0x0000FF00) | 
           ((value << 8)  & 0x00FF0000) | 
           ((value << 24) & 0xFF000000);
}

/* Custom byte-swap function for a 32-bit float */
float byteswap_float(float value) {
    uint32_t temp = *(uint32_t*)&value;
    temp = ((temp >> 24) & 0x000000FF) | 
           ((temp >> 8)  & 0x0000FF00) | 
           ((temp << 8)  & 0x00FF0000) | 
           ((temp << 24) & 0xFF000000);
    return *(float*)&temp;
}

/* load a PulseqShapeArbitrary struct from file */
void readarbitrary(PulseqShapeArbitrary* shape, FILE* fid, const short complex, int byteswap, int regular_raster) {
	int i;

	/* Read number of samples and raster (ignored if raster is irregular) */
    fread(&shape->nSamples, sizeof(int), 1, fid);
    fread(&shape->raster, sizeof(float), 1, fid);

	/* Perform byte swap if needed */
	if (byteswap == 1) {
        shape->nSamples = byteswap_int(shape->nSamples);
		shape->raster = byteswap_float(shape->raster);
    }

	/* If raster is irregular, read time coordinates */
	if (regular_raster == 0) {
		shape->time = (float*)AllocNode(sizeof(float) * shape->nSamples);
		fread(shape->time, sizeof(float), shape->nSamples, fid);
		
		/* Perform byte swap if needed */
		if (byteswap == 1) {
			for (i = 0; i < shape->nSamples; i++) {
				shape->time[i] = byteswap_float(shape->time[i]);
			}
		}
		
	}
    
    /* Read waveform magnitude */
    shape->magnitude = (float*)AllocNode(sizeof(float)*shape->nSamples);
    fread(shape->magnitude, sizeof(float), shape->nSamples, fid);

	/* Perform byte swap if needed */
	if (byteswap == 1) {
		for (i = 0; i < shape->nSamples; i++) {
			shape->magnitude[i] = byteswap_float(shape->magnitude[i]);
		}
	}

	/* If waveform is complex, read its phase */
    if (complex == 1) {
		shape->phase = (float*)AllocNode(sizeof(float) * shape->nSamples);
		fread(shape->phase, sizeof(float), shape->nSamples, fid);
		
		/* Perform byte swap if needed */
		if (byteswap == 1) {
			for (i = 0; i < shape->nSamples; i++) {
				shape->phase[i] = byteswap_float(shape->phase[i]);
			}
		}	
	}
}

/* load a PulseqRF struct from file */
void readrf(PulseqRF* rf, FILE* fid, int byteswap) {

	/* Read type. 0: empty; 1: rf; 2: extended trap rf */
	fread(&(rf->type), sizeof(short), 1, fid);
			
	/* Perform byte swap if needed */
	if (byteswap == 1) {
        rf->type = byteswap_short(rf->type);
    }

	switch (rf->type) {
		case 0:
			break;
		case 1:
			/* Read complexity flag */
			fread(&(rf->complexflag), sizeof(short), 1, fid);
					
			/* Perform byte swap if needed */
			if (byteswap == 1) {
				rf->complexflag = byteswap_short(rf->complexflag);
			}

			fprintf(stderr, "\trf->complexflag = %d\n", rf->complexflag);

			/* Read waveform */
			readarbitrary(&(rf->wav), fid, rf->complexflag, byteswap, 1);

			/* Read duration, delay, and normalized energy */
			fread(&(rf->duration), sizeof(float), 1, fid);
			fread(&(rf->delay),    sizeof(float), 1, fid);
			fread(&(rf->energy),   sizeof(float), 1, fid);

			/* Perform byte swap if needed */	
			if (byteswap == 1) {
				rf->duration = byteswap_float(rf->duration);
				rf->delay = byteswap_float(rf->delay);
				rf->energy = byteswap_float(rf->energy);
			}

			break;
		case 2:
			/* Read complexity flag */
			fread(&(rf->complexflag), sizeof(short), 1, fid);
					
			/* Perform byte swap if needed */
			if (byteswap == 1) {
				rf->complexflag = byteswap_short(rf->complexflag);
			}

			/* Read waveform */
			readarbitrary(&(rf->wav), fid, rf->complexflag, byteswap, 0);
			
			/* Read duration, delay, and normalized energy */
			fread(&(rf->duration), sizeof(float), 1, fid);
			fread(&(rf->delay),    sizeof(float), 1, fid);
			fread(&(rf->energy),   sizeof(float), 1, fid);

			/* Perform byte swap if needed */	
			if (byteswap == 1) {
				rf->duration = byteswap_float(rf->duration);
				rf->delay = byteswap_float(rf->delay);
				rf->energy = byteswap_float(rf->energy);
			}

			break;
	}
}

/* load a PulseqGrad struct from file */
void readgrad(PulseqGrad* grad, FILE* fid, int byteswap) {

	/* read type. 0: empty; 1: trap; 2: arbitrary, 3: extended trap */
	fread(&(grad->type), sizeof(short), 1, fid);

	/* Perform byte swap if needed */
	if (byteswap == 1) {
        grad->type = byteswap_short(grad->type);
    }

	/* Read waveform */
	switch (grad->type) {
		case 0:
			break;
		case 1:
			fread(&(grad->delay), sizeof(float), 1, fid);
			fread(&((grad->shape).trap.riseTime),  sizeof(float), 1, fid);
			fread(&((grad->shape).trap.flatTime),  sizeof(float), 1, fid);
			fread(&((grad->shape).trap.fallTime),  sizeof(float), 1, fid);

			/* Perform byte swap if needed */	
			if (byteswap == 1) {
				grad->delay = byteswap_float(grad->delay);
				(grad->shape).trap.riseTime = byteswap_float((grad->shape).trap.riseTime);
				(grad->shape).trap.flatTime = byteswap_float((grad->shape).trap.flatTime);
				(grad->shape).trap.fallTime = byteswap_float((grad->shape).trap.fallTime);

			}
			break;
		case 2:
			fread(&(grad->delay), sizeof(float), 1, fid);

			/* Perform byte swap if needed */
			if (byteswap == 1) {
				grad->delay = byteswap_float(grad->delay);
			}

			readarbitrary(&((grad->shape).wav), fid, 0, byteswap, 1);
			break;
		case 3:
			fread(&(grad->delay), sizeof(float), 1, fid);

			/* Perform byte swap if needed */
			if (byteswap == 1) {
				grad->delay = byteswap_float(grad->delay);
			}

			readarbitrary(&((grad->shape).wav), fid, 0, byteswap, 0);
			break;
	}
}

/* load a PulseqADC struct from file */
void readadc(PulseqADC* adc, FILE* fid, int byteswap) {

	/* read type. 0: empty; 1: adc */
	fread(&(adc->type), sizeof(short), 1, fid);

	/* Perform byte swap if needed */
	if (byteswap == 1) {
        adc->type = byteswap_short(adc->type);
    }

	switch (adc->type) {
		case 0:
			break;
		case 1:
			fread(&(adc->nSamples),  sizeof(int), 1, fid);
			fread(&(adc->dwell),     sizeof(float), 1, fid);
			fread(&(adc->delay),     sizeof(float), 1, fid);

			/* Perform byte swap if needed */
			if (byteswap == 1) {
                adc->nSamples = byteswap_int(adc->nSamples);
                adc->dwell = byteswap_float(adc->dwell);
                adc->delay = byteswap_float(adc->delay);
            }
			break;
	}
}


/** 
 * Load ParentBlock from buffer. 
 *
 * @param[in,out] seq The SegmentedSequence structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap ParentBlock elements.
 */
void readblock(Segment* segment, FILE* fid, int byteswap) {
   /* Read block ID */
  	fread(&((seq->parentBlocks[blockIdx]).ID), sizeof(int), 1, fid);
  	if (byteswap == 1) {
  		(seq->parentBlocks[blockIdx]).ID = byteswap_int((seq->parentBlocks[blockIdx]).ID);
    }
    /* Read block Duration in Block raster units */
  	fread(&((seq->parentBlocks[blockIdx]).duration_ru), sizeof(int), 1, fid);
  	if (byteswap == 1) {
      	(seq->parentBlocks[blockIdx]).duration_ru = byteswap_int((seq->parentBlocks[blockIdx]).duration_ru);
    }
    /* Read block RF event */
  	readrf(&((seq->parentBlocks[blockIdx]).rf), fid, byteswap);
  	/* Read block Gx event */
  	readgrad(&((seq->parentBlocks[blockIdx]).gx), fid, byteswap);
  	/* Read block Gy event */
  	readgrad(&((seq->parentBlocks[blockIdx]).gy), fid, byteswap);
  	/* Read block Gz event */
  	readgrad(&((seq->parentBlocks[blockIdx]).gz), fid, byteswap);
  	/* Read block ADC event */
  	readadc(&((seq->parentBlocks[blockIdx]).adc), fid, byteswap);
  	/* Read block Trigger event */
  	readtrig(&((seq->parentBlocks[blockIdx]).trig), fid, byteswap);
  	/* Read block integer User parameters */
  	fread(&((seq->parentBlocks[blockIdx]).nUserInt), sizeof(int), 1, fid);
  	if (byteswap == 1) {
      	(seq->parentBlocks[blockIdx]).nUserInt = byteswap_int((seq->parentBlocks[blockIdx]).nUserInt);
   }
   (seq->parentBlocks[blockIdx]).userInt = (int*)AllocNode(sizeof(int) * (seq->parentBlocks[blockIdx]).nUserInt);	
   fread(&((seq->parentBlocks[blockIdx]).userInt), sizeof(int), (seq->parentBlocks[blockIdx]).nUserInt, fid);
   if (byteswap == 1) {
       for (userIdx = 0; userIdx < (seq->parentBlocks[blockIdx]).nUserInt; userIdx++)  {
   		(seq->parentBlocks[blockIdx]).userInt[userIdx] = byteswap_int((seq->parentBlocks[blockIdx]).userInt[userIdx]);
   	}
   }
   /* Read block floating User parameters */
   fread(&((seq->parentBlocks[blockIdx]).nUserFloat), sizeof(int), 1, fid);
  	if (byteswap == 1) {
  		(seq->parentBlocks[blockIdx]).nUserFloat = byteswap_int((seq->parentBlocks[blockIdx]).nUserFloat);
   }
   (seq->parentBlocks[blockIdx]).userFloat = (float*)AllocNode(sizeof(float) * (seq->parentBlocks[blockIdx]).nUserFloat);	
   fread(&((seq->parentBlocks[blockIdx]).userFloat), sizeof(float), (seq->parentBlocks[blockIdx]).nUserFloat, fid);
   if (byteswap == 1) {
       for (userIdx = 0; userIdx < (seq->parentBlocks[blockIdx]).nUserFloat; userIdx++)  {
   		(seq->parentBlocks[blockIdx]).userFloat[userIdx] = byteswap_float((seq->parentBlocks[blockIdx]).userFloat[userIdx]);
   	}
  }
}

 
/** 
 * Load Segment defintion from buffer. 
 *
 * @param[in,out] seq The SegmentedSequence structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap segment definition.
 */
void readsegment(Segment* segment, FILE* fid, int byteswap) {
	int blockIdx; /* Block index */
    int userIdx;  /* User variable index */

    fread(&(segment->segmentID), sizeof(short), 1, fid);
    if (byteswap == 1) {
        segment->segmentID = byteswap_short(segment->segmentID);
    }
    fread(&(segment->nBlocksInSegment), sizeof(short), 1, fid);
	if (byteswap == 1) {
		segment->nBlocksInSegment = byteswap_short(segment->nBlocksInSegment);
    }
    segment->blockIDs = (short*)AllocNode(sizeof(short) * segment->nBlocksInSegment);
    fread(segment->blockIDs, sizeof(short), segment->nBlocksInSegment, fid);
	if (byteswap == 1) {
		for (blockIdx = 0; i < segment->nBlocksInSegment; i++) {
			segment->blockIDs[blockIdx] = byteswap_short(segment->blockIDs[blockIdx]);
		}
	}
	
	/* Read segment integer User parameters */
    fread(&(segment->nUserInt, sizeof(int), 1, fid);
	if (byteswap == 1) {
		segment->nUserInt = byteswap_int(segment->nUserInt);
    }
    segment->userInt = (int*)AllocNode(sizeof(int) * segment->nUserInt);	
    fread(&(segment->userInt), sizeof(int), segment->nUserInt), fid);
    if (byteswap == 1) {
        for (userIdx = 0; userIdx < seq->nUserInt; userIdx++)  {
    		segment->userInt[userIdx] = byteswap_int(segment->userInt[userIdx]);
    	}
    }
    /* Read segment floating User parameters */
    fread(&(segment->nUserFloat, sizeof(int), 1, fid);
	if (byteswap == 1) {
		segment->nUserFloat = byteswap_int(segment->nUserFloat);
    }
    segment->userFloat = (float*)AllocNode(sizeof(float) * segment->nUserFloat);	
    fread(&(segment->userFloat), sizeof(float), segment->nUserFloat), fid);
    if (byteswap == 1) {
        for (userIdx = 0; userIdx < seq->nUserInt; userIdx++)  {
    		segment->userFloat[userIdx] = byteswap_float(segment->userFloat[userIdx]);
    	}
    }
}

/** 
 * Load a scan Loop array from buffer. 
 *
 * @param[in,out] seq The SegmentedSequence structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap loop array elements.
 */
void readloop(seq* seq, FILE* fid, int byteswap) {
    int rowIdx;
	int colIdx;

    seq->loop = (float**)AllocNode(sizeof(float*) * seq->nRowsInLoopArray);
    for (n = 0; n < seq->nRowsInLoopArray; n++) {
        seq->loop[rowIdx] = (float*)AllocNode(sizeof(float) * seq->nColumnsInLoopArray);
        fread(seq->loop[rowIdx], sizeof(float), seq->nColumnsInLoopArray, fid);
		if (byteswap == 1) {
			for (colIdx = 0; colIdx < seq->nColumnsInLoopArray; colIdx++) {
				seq->loop[rowIdx][colIdx] = byteswap_float(seq->loop[rowIdx][colIdx]);
			}
		}
    }
}

/** 
 * Load a SegmentedSequence struct from buffer. 
 *
 * @param[out] seq The SegmentedSequence structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 */
void read_seq_frombuffer(SegmentedSequence* seq, FILE* fid) { 
	int segIdx;   /* Segment index */
    int blockIdx; /* Parent block index */
    int userIdx;  /* User variable index */
    int colIdx;  /* Loop Column index */

    /* Check endianness */
    short byteswap;
	fread(&byteswap, sizeof(short), 1, fid);
    byteswap = byteswap == 47 ? 0 : 1 ;
    
    /* Get combined Pulseq version */
	fread(&(seq->version_combined), sizeof(int), 1, fid);
	if (byteswap == 1) {
		seq->version_combined = byteswap_int(seq->version_combined);
    }
    
	/* Read parent blocks*/
	fread(&(seq->nParentBlocks), sizeof(short), 1, fid);
	if (byteswap == 1) {
		seq->nParentBlocks = byteswap_short(seq->nParentBlocks);
    }
	seq->parentBlocks = (PulseqBlock*)AllocNode(sizeof(PulseqBlock) * seq->nParentBlocks);
    for (blockIdx = 0; blockIdx < seq->nParentBlocks; blockIdx++)  {
           
    }	
		
	/* Read Segments */
	fread(&(seq->nSegments), sizeof(short), 1, fid);
	if (byteswap == 1) {
        seq->nSegments = byteswap_short(seq->nSegments);
    }
	seq->segments = (Segment*)AllocNode(sizeof(Segment) * seq->nSegments);
    for (i = 0; i < seq->nSegments; i++) {
        readsegment(&(seq->segments[i]), fid, byteswap);
    }

	/* Read Scan Loop */
	fread(&(seq->nRowsInLoopArray), sizeof(int), 1, fid);
	if (byteswap == 1) {
		seq->nRowsInLoopArray = byteswap_int(seq->nMax);
    }
	fread(&(seq->nColumnsInLoopArray), sizeof(short), 1, fid);	
	if (byteswap == 1) {
		seq->nColumnsInLoopArray = byteswap_short(seq->nColumnsInLoopArray);
    }
    seq->columnIdx = (short*)AllocNode(sizeof(short) * seq->nColumnsInLoopArray);	
    fread(&(seq->columnIdx), sizeof(short), seq->nColumnsInLoopArray, fid);
    if (byteswap == 1) {
        for (colIdx = 0; colIdx < seq->nColumnsInLoopArray; colIdx++)  {
    		seq->columnIdx[colIdx] = byteswap_short(seq->columnIdx[colIdx]);
    	}
    }
    readloop(seq, fid, byteswap);
    
    /* Read raster times */
    fread(&(seq->adc_raster_us), sizeof(float), 1, fid);
	if (byteswap == 1) {
		seq->adc_raster_us = byteswap_float(seq->adc_raster_us);
    }
    fread(&(seq->grad_raster_us), sizeof(float), 1, fid);
	if (byteswap == 1) {
		seq->grad_raster_us = byteswap_float(seq->grad_raster_us);
    }
    fread(&(seq->rf_raster_us), sizeof(float), 1, fid);
	if (byteswap == 1) {
		seq->rf_raster_us = byteswap_float(seq->rf_raster_us);
    }
    fread(&(seq->block_duration_raster_us), sizeof(float), 1, fid);
	if (byteswap == 1) {
		seq->block_duration_raster_us = byteswap_float(seq->block_duration_raster_us);
    }
    
    /* Read sequence integer User parameters */
    fread(&(seq->nUserInt, sizeof(int), 1, fid);
	if (byteswap == 1) {
		seq->nUserInt = byteswap_int(seq->nUserInt);
    }
    seq->userInt = (int*)AllocNode(sizeof(int) * seq->nUserInt);	
    fread(&(seq->userInt), sizeof(int), seq->nUserInt), fid);
    if (byteswap == 1) {
        for (userIdx = 0; userIdx < seq->nUserInt; userIdx++)  {
    		seq->userInt[userIdx] = byteswap_int(seq->userInt[userIdx]);
    	}
    }
    /* Read sequence floating User parameters */
    fread(&(seq->nUserFloat, sizeof(int), 1, fid);
	if (byteswap == 1) {
		seq->nUserFloat = byteswap_int(seq->nUserFloat);
    }
    seq->userFloat = (float*)AllocNode(sizeof(float) * seq->nUserFloat);	
    fread(&(seq->userFloat), sizeof(float), seq->nUserFloat), fid);
    if (byteswap == 1) {
        for (userIdx = 0; userIdx < seq->nUserInt; userIdx++)  {
    		seq->userFloat[userIdx] = byteswap_float(seq->userFloat[userIdx]);
    	}
    }
}

/** 
 * Load a SegmentedSequence struct from file. 
 *
 * @param[out] seq The SegmentedSequence structure to be filled from file
 * @param[in] filename File containing serialized SegmentedSequence
 */
void read_seq_fromfile(SegmentedSequence* seq, const char* filename) { 
    FILE *fid;
	if ((fid = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "%s not found\n", filename);
		return;
	}

	read_seq_frombuffer(seq, fid);
	fclose(fid);
}