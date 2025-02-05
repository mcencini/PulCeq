/**
 * @file pulSeg.c
 */
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "sdkver.h" 
 
#include "pulSeg.h"

/****************************************************************/ 
/*         Short, Int, Float scalar and array reading           */
/****************************************************************/ 
/**
 * @brief Swaps the byte order of a short (16-bit integer).
 * 
 * @param[in] val The short value to be byte-swapped.
 * @return The byte-swapped short value.
 */
short byteswap_short(short value) {
    return (short)(((value >> 8) & 0x00FF) | 
                   ((value << 8) & 0xFF00));
}

/**
 * @brief Swaps the byte order of an int (32-bit integer).
 * 
 * @param[in] val The int value to be byte-swapped.
 * @return The byte-swapped int value.
 */
int byteswap_int(int value) {
    return ((value >> 24) & 0x000000FF) | 
           ((value >> 8)  & 0x0000FF00) | 
           ((value << 8)  & 0x00FF0000) | 
           ((value << 24) & 0xFF000000);
}

/**
 * @brief Swaps the byte order of a float (32-bit floating point).
 * 
 * @param val[in]  The float value to be byte-swapped.
 * @return The byte-swapped float value.
 */
float byteswap_float(float value) {
    uint32_t temp = *(uint32_t*)&value;
    temp = ((temp >> 24) & 0x000000FF) | 
           ((temp >> 8)  & 0x0000FF00) | 
           ((temp << 8)  & 0x00FF0000) | 
           ((temp << 24) & 0xFF000000);
    return *(float*)&temp;
}

/**
 * @brief Reads one or more short (16-bit integer) values from a file.
 * 
 * @param[in, out] value Pointer to the memory where the read values will be stored.
 * @param[in] fid File pointer to read from.
 * @param[in] byteswap If 1, perform byte-swapping for endianness correction.
 * @param[in] count Number of short values to read.
 * @return 0 on success, -1 on failure.
 */
void read_short(short* value, FILE* fid, const short byteswap, const int count) {
    fread(value, sizeof(short), count, fid);
    if (byteswap) {
        for (int i = 0; i < count; i++) {
            value[i] = byteswap_int(value[i]);
        }
    }
}

/**
 * @brief Reads one or more int (32-bit integer) values from a file.
 * 
 * @param[in, out] value Pointer to the memory where the read values will be stored.
 * @param[in] fid File pointer to read from.
 * @param[in] byteswap If 1, perform byte-swapping for endianness correction.
 * @param[in] count Number of short values to read.
 */
void read_int(int* value, FILE* fid, const short byteswap, const int count) {
    fread(value, sizeof(int), count, fid);
    if (byteswap) {
        for (int i = 0; i < count; i++) {
            value[i] = byteswap_int(value[i]);
        }
    }
}

/**
 * @brief Reads one or more int (32-bit floating point) values from a file.
 * 
 * @param[in, out] value Pointer to the memory where the read values will be stored.
 * @param[in] fid File pointer to read from.
 * @param[in] byteswap If 1, perform byte-swapping for endianness correction.
 * @param[in] count Number of short values to read.
 */
void read_float(float* value, FILE* fid, const short byteswap, const int count) {
    fread(value, sizeof(float), count, fid);
    if (byteswap) {
        for (int i = 0; i < count; i++) {
            value[i] = byteswap_int(value[i]);
        }
    }
}

/****************************************************************/ 
/*                   Normalized shapes reading                  */
/****************************************************************/ 
/** 
 * @brief Reads Arbitrary shape from buffer. 
 *
 * @param[in,out] shape The PulseqShapeArbitrary structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap PulseqShapeArbitrary elements.
 */
void read_arbitrary(PulseqShapeArbitrary* shape, FILE* fid, const short byteswap) {
	/* Read number of samples */
    read_int(&(shape->nSamples), fid, byteswap, 1);

    /* Read waveform */
    shape->samples = (float*)AllocNode(sizeof(float) * shape->nSamples);
    read_float(shape->samples, fid, byteswap, shape->nSamples);
}

/** 
 * @brief Reads Trapezoid shape from buffer. 
 *
 * @param[in,out] shape The PulseqShapeTrap structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap PulseqShapeTrap elements.
 */
void read_trap(PulseqShapeTrap* shape, FILE* fid, const short byteswap) {
	/* Read rise, flat and fall times of the trapezoid */
    read_int(&(shape->riseTime), fid, byteswap, 1);
    read_int(&(shape->flatTime), fid, byteswap, 1);
    read_int(&(shape->fallTime), fid, byteswap, 1);
}

/****************************************************************/ 
/*                  Pulseq event reading                        */
/****************************************************************/ 
/** 
 * @brief Reads RF event from buffer. 
 *
 * @param[in,out] rf The PulseqRF event to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap RF elements.
 */
void read_rf(PulseqRF* rf, FILE* fid, const short byteswap) {
	/* Read type. 0: empty; 1: rf*/
	read_short(&(rf->type), fid, byteswap, 1);

	switch (rf->type) {
		case 0:
			break;
		case 1:
			/* Read magnitude part */
			read_arbitrary(&(rf->magShape), fid, byteswap);

			/* Read phase part */
			read_arbitrary(&(rf->phaseShape), fid, byteswap);

			/* Read time part */
			read_arbitrary(&(rf->timeShape), fid, byteswap);

			/* Read delay */
			read_int(&(rf->delay), fid, byteswap, 1);

			/* Read RF event integer User parameters */
			read_int(&(rf->nUserInt), fid, byteswap, 1);
			rf->userInt = (int*)AllocNode(sizeof(int) * rf->nUserInt);
			read_int(rf->userInt, fid, byteswap, (int)rf>nUserInt);	

			/* Read RF event floating point User parameters */
			read_int(&(rf->nUserFloat), fid, byteswap, 1);
			rf->userFloat = (float*)AllocNode(sizeof(float) * rf->nUserFloat);	
			read_float(rf->userFloat, fid, byteswap, (int)rf->nUserFloat);

			break;
	}
}

/** 
 * @brief Reads Grad event from buffer. 
 *
 * @param[in,out] grad The PulseqGrad event to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap Grad elements.
 */
void read_grad(PulseqGrad* grad, FILE* fid, const short byteswap) {
	/* Read type. 0: empty; 1: trap; 2: arbitrary, 3: extended trap */
	read_short(&(grad->type), fid, byteswap, 1);
	
	switch (grad->type) {
		case 0:
			break;
		case 1:
			/* Read delay */
			read_int(&(rf->delay), fid, byteswap, 1);

			/* Read trapezoid */
			read_trap(&(rf->trap), fid, byteswap)
			
			break;
		case 2:
			/* Read delay */
			read_int(&(rf->delay), fid, byteswap, 1);

			/* Read waveform part */
			read_arbitrary(&(rf->waveShape), fid, byteswap);

			/* Read time part */
			read_arbitrary(&(rf->timeShape), fid, byteswap);

			break;
		case 3:
			/* Read delay */
			read_int(&(rf->delay), fid, byteswap, 1);

			/* Read waveform part */
			read_arbitrary(&(rf->waveShape), fid, byteswap);

			/* Read time part */
			read_arbitrary(&(rf->timeShape), fid, byteswap);
		
			break;
	}
}

/** 
 * @brief Reads ADC event from buffer. 
 *
 * @param[in,out] adc The PulseqADC event to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap ADC elements.
 */
void read_adc(PulseqADC* adc, FILE* fid, int byteswap) {
	/* read type. 0: empty; 1: adc */
	read_short(&(adc->type), fid, byteswap, 1);

	switch (adc->type) {
		case 0:
			break;
		case 1:
			read_int(&(adc->numSamples), fid, byteswap, 1);
			read_int(&(adc->dwell), fid, byteswap, 1);
			read_int(&(adc->delay), fid, byteswap, 1);

			break;
	}
}

/** 
 * @brief Reads Trigger event from buffer. 
 *
 * @param[in,out] trig The PulseqTrigger event to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap Trigger elements.
 */
void read_trig(PulseqTrigger* trig, FILE* fid, int byteswap) {
	/* read type. 0: OFF; 1: ON */
	read_short(&(trig->type), fid, byteswap, 1);

	switch (adc->type) {
		case 0:
			break;
		case 1:
			read_int(&(trig->duration), fid, byteswap, 1);
			read_int(&(trig->delay), fid, byteswap, 1);
			read_int(&(trig->triggerType), fid, byteswap, 1);
			read_int(&(trig->triggerChannel), fid, byteswap, 1);

			break;
	}
}

/*********************************************************************************************************/ 
/*                             Block, Segment(s), Loop, and Sequence reading                             */
/*********************************************************************************************************/ 
/** 
 * @brief Reads ParentBlock array from buffer. 
 *
 * @param[in,out] parentBlocks The PulseqBlock array to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap ParentBlock elements.
 */
void read_blocks(PulseqBlock* parentBlocks, FILE* fid, const short byteswap, const short nParentBlocks) {
	int blockIdx;

	/* Loop over Parent Blocks */
	for (blockIdx = 0; blockIdx < nParentBlocks; blockIdx++) {
		/* Read block ID */
		read_int(&(parentBlocks[blockIdx]->ID), fid, byteswap, 1);

		/* Read block Duration in Block raster units */
		read_int(&(parentBlocks[blockIdx]->duration_ru), fid, byteswap, 1);

		/* Read block RF event */
		read_rf(&(parentBlocks[blockIdx]->rf), fid, byteswap);

		/* Read block Gx event */
		read_grad(&(parentBlocks[blockIdx]->gx), fid, byteswap);
		/* Read block Gy event */
		read_grad(&(parentBlocks[blockIdx]->gy), fid, byteswap);
		/* Read block Gz event */
		read_grad(&(parentBlocks[blockIdx]->gz), fid, byteswap);

		/* Read block ADC event */
		read_adc(&(parentBlocks[blockIdx]->adc), fid, byteswap);

		/* Read block Trigger event */
		read_trig(&(parentBlocks[blockIdx]->trig), fid, byteswap);

		/* Read block integer User parameters */
		read_int(&(parentBlocks[blockIdx]->nUserInt), fid, byteswap, 1);
		parentBlocks[blockIdx]->userInt = (int*)AllocNode(sizeof(int) * parentBlocks[blockIdx]->nUserInt);
		read_int(parentBlocks[blockIdx]->userInt, fid, byteswap, (int)parentBlocks[blockIdx]->nUserInt);	

		/* Read sequence floating point User parameters */
		read_int(&(parentBlocks[blockIdx]->nUserFloat), fid, byteswap, 1);
		parentBlocks[blockIdx]->userFloat = (float*)AllocNode(sizeof(float) * parentBlocks[blockIdx]->nUserFloat);	
		read_float(parentBlocks[blockIdx]->userFloat, fid, byteswap, (int)parentBlocks[blockIdx]->nUserFloat);
	}
}
 
/** 
 * Load Segment defintion from buffer. 
 *
 * @param[in,out] seq The SegmentedSequence structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap segment definition.
 */
void read_segments(Segment* segment, FILE* fid, const short byteswap, const short nSegments) {
	int segIdx;	

	/* Loop over Segments */
	for (segIdx = 0; segIdx < nSegments; segIdx++) {
		read_short(&(segment[segIdx]->segmentID), fid, byteswap, 1);

		/* Read Segment definition */
		read_short(&(segment[segIdx]->nBlocksInSegment), fid, byteswap, 1);
		segment[segIdx]->blockIDs = (short*)AllocNode(sizeof(short) * segment[segIdx]->nBlocksInSegment);
		read_short(segment[segIdx]->blockIDs, fid, byteswap, (int)segment[segIdx]->nBlocksInSegment);
		
		/* Read Segment event integer User parameters */
		read_int(&(segment[segIdx]->nUserInt), fid, byteswap, 1);
		segment[segIdx]->userInt = (int*)AllocNode(sizeof(int) * segment[segIdx]->nUserInt);
		read_int(segment[segIdx]->userInt, fid, byteswap, (int)rf>nUserInt);	

		/* Read RF event floating point User parameters */
		read_int(&(segment[segIdx]->nUserFloat), fid, byteswap, 1);
		segment[segIdx]->useFloat = (int*)AllocNode(sizeof(int) * segment[segIdx]->nUserFloat);
		read_float(segment[segIdx]->useFloat, fid, byteswap, (int)segment[segIdx]->nUserFloat);
	}
}

/** 
 * @brief Reads a scan Loop array from buffer. 
 *
 * @param[in,out] seq The SegmentedSequence structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 * @param[in] byteswap Flag to byteswap loop array elements.
 */
void read_loop(seq* seq, FILE* fid, const short byteswap) {
    int rowIdx;

	/* Read loop matrix size (nRows, nCols) */
	read_int(&((seq->loop).nRowsInLoopArray), fid, byteswap, 1);
	read_short(&((seq->loop).nColumnsInLoopArray), fid, byteswap, 1);	

	/* Read non-empty column indexes */
    (seq->loop).columnIdx = (short*)AllocNode(sizeof(short) * (seq->loop).nColumnsInLoopArray);	
    read_short((seq->loop).columnIdx, fid, byteswap, (int)(seq->loop).nColumnsInLoopArrayy);

	/* Read loop values*/
    (seq->loop).values = (float**)AllocNode(sizeof(float*) * (seq->loop).nColumnsInLoopArray);
    for (rowIdx = 0; rowIdx < (seq->loop).nRowsInLoopArray; rowIdx++) {
        (seq->loop).values[rowIdx] = (float*)AllocNode(sizeof(float) * (seq->loop).nColumnsInLoopArray);
        read_float((seq->loop).values[rowIdx], fid, byteswap, (int)(seq->loop).nColumnsInLoopArray);
    }
}

/** 
 * @brief Reads a SegmentedSequence struct from buffer. 
 *
 * @param[out] seq The SegmentedSequence structure to be filled from file
 * @param[in] fid Buffer containing serialized SegmentedSequence
 */
void read_seq_frombuffer(SegmentedSequence* seq, FILE* fid) {
    /* Check endianness */
    short byteswap;
	read_short(&byteswap, fid, 0, 1);
    byteswap = byteswap == 47 ? 0 : 1 ;
    
    /* Get combined Pulseq version */
	read_int(&(seq->version_combined), fid, byteswap, 1);
    
	/* Read parent blocks*/
	read_short(&(seq->nParentBlocks), fid, byteswap, 1);
	seq->parentBlocks = (PulseqBlock*)AllocNode(sizeof(PulseqBlock) * seq->nParentBlocks);
	read_blocks(seq->parentBlocks, fid, byteswap, seq->nParentBlocks);
		
	/* Read Segments */
	read_int(&(seq->nSegments), fid, byteswap, 1);
	seq->segments = (Segment*)AllocNode(sizeof(Segment) * seq->nSegments);
	read_segments(seq->segments, fid, byteswap, seq->nSegments);

	/* Read Scan Loop */
    read_loop(seq, fid, byteswap);
    
    /* Read raster times */
    read_float(&(seq->adc_raster_us), fid, byteswap, 1);
    read_float(&(seq->grad_raster_us), fid, byteswap, 1);
    read_float(&(seq->rf_raster_us), fid, byteswap, 1);
    read_float(&(seq->block_duration_raster_us), fid, byteswap, 1);
    
    /* Read sequence integer User parameters */
    read_int(&(seq->nUserInt), fid, byteswap, 1);
    seq->userInt = (int*)AllocNode(sizeof(int) * seq->nUserInt);
	read_int(seq->userInt, fid, byteswap, seq->nUserInt);	

    /* Read sequence floating point User parameters */
    read_int(&(seq->nUserFloat), fid, byteswap, 1);
    seq->userFloat = (float*)AllocNode(sizeof(float) * seq->nUserFloat);	
	read_float(seq->userFloat, fid, byteswap, seq->nUserFloat);	
}


/** 
 * @brief Reads a SegmentedSequence struct from file. 
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

