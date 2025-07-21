/**
 * @file block.c
 * @brief Implementation of Pulseq block definition and handling Pulseq blocks.
 *
 */

#include <math.h>
#include <stddef.h>

#include "seqfile.h"

#include "alloc.h"
#include "block.h"
#include "config.h"
#include "constants.h"

/*********************************************************  local utils  *********************************************************/
#define MAX_EXTENSIONS_PER_BLOCK 64

/**
 * @struct RawBlock
 * @brief  Raw block content IDs and extension data.
 *
 * This structure holds the content IDs of a block and its extensions.
 * It is used to retrieve the raw data from the sequence file.
 */
typedef struct {
    int block_duration;
    int rf;
    int gx;
    int gy;
    int gz;
    int adc;
    int extCount;
    int ext[MAX_EXTENSIONS_PER_BLOCK][2];  /* [type, ref] */
} RawBlock;

/**
 * @brief Get the raw block content IDs from the sequence file.
 *
 * @param seq Pointer to the SeqFile structure.
 * @param blockIndex Index of the block to retrieve.
 * @param parseExtensions Flag indicating whether to parse extensions.
 * @return RawBlock containing the block's content IDs and extension data.
 */
RawBlock getRawBlockContentIDs(const SeqFile* seq, int blockIndex, int parseExtensions)
{
    RawBlock block;
    int i, nextExtID, extCount;
    float* eventFloat;
    float* extData;

    /* Initialize */
    block.block_duration = 0;
    block.rf = 0;
    block.gx = 0;
    block.gy = 0;
    block.gz = 0;
    block.adc = 0;
    block.extCount = 0;

    /* Sanity check */
    if (seq == 0 || blockIndex < 0 || blockIndex >= seq->numBlocks) {
        return block;
    }

    /* Access float data row and cast entries to int */
    eventFloat = seq->blockLibrary[blockIndex];

    int duration = (int)(eventFloat[0]);
    int rfID = (int)(eventFloat[1]) - 1;
    int gxID = (int)(eventFloat[2]) - 1;
    int gyID = (int)(eventFloat[3]) - 1;
    int gzID = (int)(eventFloat[4]) - 1;
    int adcID  = (int)(eventFloat[5]) - 1;
    int extID = (int)(eventFloat[6]) - 1;

    block.block_duration = duration;
    block.rf = rfID;
    block.gx = gxID;
    block.gy = gyID;
    block.gz = gzID;
    block.adc = adcID;

    /* Handle extensions if present */
    if (parseExtensions && extID >= 0 && seq->isExtensionsLibraryParsed) {
        nextExtID = extID;
        extCount = 0;

        while (
            nextExtID >= 0 &&
            nextExtID < seq->extensionsLibrarySize &&
            extCount < MAX_EXTENSIONS_PER_BLOCK
        ) {
            extData = seq->extensionsLibrary[nextExtID]; /* [type, ref, next_id] */
            block.ext[extCount][0] = (int)extData[0];  /* type */
            block.ext[extCount][1] = (int)extData[1];  /* ref */
            nextExtID = (int)extData[2] - 1; /* next in chain */
            extCount += 1;
        }

        block.extCount = extCount;
    }

    return block;
}
/*********************************************************  end local utils  *********************************************************/

SeqBlock* __seqBlock(void)
{
    SeqBlock* block = (SeqBlock*)ALLOC(sizeof(SeqBlock));
    RFEvent rf;
    GradEvent gx;
    GradEvent gy;
    GradEvent gz;
    ADCEvent adc;
    TriggerEvent trigger;
    RotationEvent rotation;
    LabelEvent labelset;
    LabelEvent labelinc;
    SoftDelayEvent delay;
    RfShimmingEvent rfShimming;

    /* Initialize rf Event*/
    rf.type = 0;
    gx.type = 0;
    gy.type = 0;
    gz.type = 0;
    adc.type = 0;
    trigger.type = 0;
    rotation.type = 0;
    delay.type = 0;
    rfShimming.type = 0;

    /* Initialize all labels (set) to 0 */
    labelset.type = 0;
    labelset.slc = 0;
    labelset.seg = 0;
    labelset.rep = 0;
    labelset.avg = 0;
    labelset.set = 0;
    labelset.eco = 0;
    labelset.phs = 0;
    labelset.lin = 0;
    labelset.par = 0;
    labelset.acq = 0;
    labelset.trid = 0;
    labelset.nav = 0;
    labelset.rev = 0;
    labelset.sms = 0;
    labelset.ref = 0;
    labelset.ima = 0;
    labelset.noise = 0;
    labelset.pmc = 0;
    labelset.norot = 0;
    labelset.nopos = 0;  
    labelset.noscl = 0;     
    labelset.once = 0;     
   
    /* Initialize all labels (increment) to 0 */
    labelinc.type = 0;
    labelinc.slc = 0;
    labelinc.seg = 0;
    labelinc.rep = 0;
    labelinc.avg = 0;
    labelinc.set = 0;
    labelinc.eco = 0;
    labelinc.phs = 0;
    labelinc.lin = 0;
    labelinc.par = 0;
    labelinc.acq = 0;
    labelinc.trid = 0;
    labelinc.nav = 0;
    labelinc.rev = 0;
    labelinc.sms = 0;
    labelinc.ref = 0;
    labelinc.ima = 0;
    labelinc.noise = 0;
    labelinc.pmc = 0;
    labelinc.norot = 0;
    labelinc.nopos = 0;  
    labelinc.noscl = 0;     
    labelinc.once = 0;   

    /* Initialize the block */
    block->rf = rf;
    block->gx = gx;
    block->gy = gy;
    block->gz = gz;
    block->adc = adc;
    block->trigger = trigger;
    block->rotation = rotation;
    block->labelset = labelset;
    block->labelinc = labelinc;
    block->delay = delay;
    block->rfShimming = rfShimming;

    return block;
}

void __seqBlockFree(SeqBlock* block)
{
    if (block == 0) return;

    /* RF waveforms */
    if (block->rf.type > 0){
        if (block->rf.magShape.samples) {
            FREE(block->rf.magShape.samples);
            block->rf.magShape.samples = NULL;
        }
        if (block->rf.phaseShape.samples) {
            FREE(block->rf.phaseShape.samples);
            block->rf.phaseShape.samples = NULL;
        }
        if (block->rf.timeShape.samples) {
            FREE(block->rf.timeShape.samples);
            block->rf.timeShape.samples = NULL;
        }
    }

    /* GX waveforms */
    if (block->gx.type > 1){
        if (block->gx.waveShape.samples) {
            FREE(block->gx.waveShape.samples);
            block->gx.waveShape.samples = NULL;
        }
        if (block->gx.timeShape.samples) {
            FREE(block->gx.timeShape.samples);
            block->gx.timeShape.samples = NULL;
        }
    }

    /* GY waveforms */
    if (block->gy.type > 1){
        if (block->gy.waveShape.samples) {
            FREE(block->gy.waveShape.samples);
            block->gy.waveShape.samples = NULL;
        }
        if (block->gy.timeShape.samples) {
            FREE(block->gy.timeShape.samples);
            block->gy.timeShape.samples = NULL;
        }
    }

    /* GZ waveforms */
    if (block->gz.type > 1){
        if (block->gz.waveShape.samples) {
            FREE(block->gz.waveShape.samples);
            block->gz.waveShape.samples = NULL;
        }
        if (block->gz.timeShape.samples) {
            FREE(block->gz.timeShape.samples);
            block->gz.timeShape.samples = NULL;
        }
    }

    /* ADC waveform */
    if (block->adc.type > 0){
        if (block->adc.phaseModulationShape.samples) {
            FREE(block->adc.phaseModulationShape.samples);
            block->adc.phaseModulationShape.samples = NULL;
        }
    }

    /* RF shimming arrays */
    if (block->rfShimming.type > 0){
        if (block->rfShimming.amplitudes) {
            FREE(block->rfShimming.amplitudes);
            block->rfShimming.amplitudes = NULL;
        }
        if (block->rfShimming.phases) {
            FREE(block->rfShimming.phases);
            block->rfShimming.phases = NULL;
        }
    }
}


SeqBlock* __getBlock(const SeqFile* seq, int blockIndex, int parseExtensions) {
    SeqBlock* block = __seqBlock(); /* Initializes all event types to 0 */

    float* farray;
    int idx;
    int i, labelID, labelValue, extType, extIdx;
    int numRealSamples = 0;
    float* trig;
    float* rot;
    float* delay;
    int* isRealSample;
    RfShimEntry rfshim;
    RawBlock rawBlock = getRawBlockContentIDs(seq, blockIndex, parseExtensions);

    /* Set the duration */
    block->duration = rawBlock.block_duration;

    /* ------------------ RF Event ------------------ */
    if (rawBlock.rf >= 0) {
        farray = seq->rfLibrary[rawBlock.rf];
        block->rf.type = 1;
        block->rf.amplitude = farray[0];

        idx = (int)farray[1];
        if (idx > 0) block->rf.magShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));

        idx = (int)farray[2];
        if (idx > 0) {
            block->rf.phaseShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));
            for (i = 0; i < block->rf.phaseShape.numSamples; i++) {
                block->rf.phaseShape.samples[i] *= TWO_PI; /* Rescale phase shape to radians */
            }
        } else {
            block->rf.phaseShape.numSamples = 0; /* Set phase shape to 0 samples */
            block->rf.phaseShape.numUncompressedSamples = 0; /* Set phase shape to 0 samples */
            block->rf.phaseShape.samples = NULL; /* Free phase shape samples */
        }

        /* Attempt to detect real-valued RF waveform */
        if (DETECT_REAL_RF && block->rf.magShape.numSamples > 0 && block->rf.phaseShape.numSamples > 0) {
            isRealSample = (int*)ALLOC(block->rf.magShape.numSamples * sizeof(int));

            /* Check if the phase shape is real-valued */
            for (i = 0; i < block->rf.magShape.numSamples; i++) {
                isRealSample[i] = fabs(block->rf.phaseShape.samples[i]) < 1e-6 || fabs(block->rf.phaseShape.samples[i] - M_PI) < 1e-6;  
            }
            for (i = 0; i < block->rf.magShape.numSamples; i++) {
                if (isRealSample[i]) {
                    numRealSamples++;
                }
            }

            /* If all samples are real, set the phase shape to 0 samples and free it */
            if (numRealSamples == block->rf.magShape.numSamples) {
                block->rf.phaseShape.numSamples = 0; /* Set phase shape to 0 samples */
                block->rf.phaseShape.numUncompressedSamples = 0; /* Set phase shape to 0 samples */
                FREE(block->rf.phaseShape.samples); /* Free phase shape samples */
                block->rf.phaseShape.samples = NULL; /* Free phase shape samples */
            }
            FREE(isRealSample);
        }

        idx = (int)farray[3];
        if (idx > 0) {
            block->rf.timeShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));
        } else {
            block->rf.timeShape.numSamples = 0; /* Set time shape to 0 samples */
            block->rf.timeShape.numUncompressedSamples = 0; /* Set time shape to 0 samples */
            block->rf.timeShape.samples = NULL; /* Free time shape samples */
        }

        block->rf.center = farray[4];
        block->rf.delay = (int)farray[5];
        block->rf.freqPPM = farray[6];
        block->rf.phasePPM = farray[7];
        block->rf.freqOffset = farray[8];
        block->rf.phaseOffset = farray[9];
    }

    /* ------------------ Gradient GX ------------------ */
    if (rawBlock.gx >= 0) {
        farray = seq->gradLibrary[rawBlock.gx];
        block->gx.amplitude = farray[1];

        if ((int)farray[0] == 0) {
            block->gx.type = 1;
            block->gx.trap.riseTime = (long)farray[2];
            block->gx.trap.flatTime = (long)farray[3];
            block->gx.trap.fallTime = (long)farray[4];
            block->gx.delay = (int)farray[5];
            block->gx.first = 0;
            block->gx.last = 0;
        } else if ((int)farray[0] == 1) {
            block->gx.type = 2;
            block->gx.first = farray[2];
            block->gx.last = farray[3];

            idx = (int)farray[4];
            if (idx > 0) block->gx.waveShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));

            idx = (int)farray[5];
            if (idx > 0) {
                block->gx.timeShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));
            } else {
                block->gx.timeShape.numSamples = 0; /* Set time shape to 0 samples */
                block->gx.timeShape.numUncompressedSamples = 0; /* Set time shape to 0 samples */
                block->gx.timeShape.samples = NULL; /* Free time shape samples */
            }

            block->gx.delay = (int)farray[6];
        }
    }

    /* ------------------ Gradient GY ------------------ */
    if (rawBlock.gy >= 0) {
        farray = seq->gradLibrary[rawBlock.gy];
        block->gy.amplitude = farray[1];

        if ((int)farray[0] == 0) {
            block->gy.type = 1;
            block->gy.trap.riseTime = (long)farray[2];
            block->gy.trap.flatTime = (long)farray[3];
            block->gy.trap.fallTime = (long)farray[4];
            block->gy.delay = (int)farray[5];
            block->gy.first = 0;
            block->gy.last = 0;
        } else if ((int)farray[0] == 1) {
            block->gy.type = 2;
            block->gy.first = farray[2];
            block->gy.last = farray[3];

            idx = (int)farray[4];
            if (idx > 0) block->gy.waveShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));

            idx = (int)farray[5];
            if (idx > 0) {
                block->gy.timeShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));
            } else {
                block->gy.timeShape.numSamples = 0; /* Set time shape to 0 samples */
                block->gy.timeShape.numUncompressedSamples = 0; /* Set time shape to 0 samples */
                block->gy.timeShape.samples = NULL; /* Free time shape samples */
            }

            block->gy.delay = (int)farray[6];
        }
    }

    /* ------------------ Gradient GZ ------------------ */
    if (rawBlock.gz >= 0) {
        farray = seq->gradLibrary[rawBlock.gz];
        block->gz.amplitude = farray[1];

        if ((int)farray[0] == 0) {
            block->gz.type = 1;
            block->gz.trap.riseTime = (long)farray[2];
            block->gz.trap.flatTime = (long)farray[3];
            block->gz.trap.fallTime = (long)farray[4];
            block->gz.delay = (int)farray[5];
            block->gz.first = 0;
            block->gz.last = 0;
        } else if ((int)farray[0] == 1) {
            block->gz.type = 2;
            block->gz.first = farray[2];
            block->gz.last = farray[3];

            idx = (int)farray[4];
            if (idx > 0) block->gz.waveShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));

            idx = (int)farray[5];
            if (idx > 0) {
                block->gz.timeShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));
            } else {
                block->gz.timeShape.numSamples = 0; /* Set time shape to 0 samples */
                block->gz.timeShape.numUncompressedSamples = 0; /* Set time shape to 0 samples */
                block->gz.timeShape.samples = NULL; /* Free time shape samples */
            }

            block->gz.delay = (int)farray[6];
        }
    }

    /* ------------------ ADC Event ------------------ */
    if (rawBlock.adc >= 0) {
        farray = seq->adcLibrary[rawBlock.adc];
        block->adc.type = 1;
        block->adc.numSamples = (int)farray[0];
        block->adc.dwellTime = (int)farray[1];
        block->adc.delay = (int)farray[2];
        block->adc.freqPPM = farray[3];
        block->adc.phasePPM = farray[4];
        block->adc.freqOffset = farray[5];
        block->adc.phaseOffset = farray[6];

        idx = (int)farray[7];
        if (idx > 0) {
            block->adc.phaseModulationShape = *decompressShape(&(seq->shapesLibrary[idx - 1]));
        } else {
            block->adc.phaseModulationShape.numSamples = 0; /* Set phase modulation shape to 0 samples */
            block->adc.phaseModulationShape.numUncompressedSamples = 0; /* Set phase modulation shape to 0 samples */
            block->adc.phaseModulationShape.samples = NULL; /* Free phase modulation shape samples */
        }
    }

     /* ------------------ Extensions ------------------ */
    for (i = 0; i < rawBlock.extCount; i++) {
        extType = seq->extensionLUT[rawBlock.ext[i][0]];
        extIdx = rawBlock.ext[i][1];

        switch (extType) {
            case EXT_TRIGGER:   
                trig = seq->triggerLibrary[extIdx];
                block->trigger.type = 1;
                block->trigger.duration = (long)trig[0];
                block->trigger.delay = (long)trig[1];
                block->trigger.triggerType = (int)trig[2];
                block->trigger.triggerChannel = (int)trig[3];
                break;
            case EXT_ROTATION:
                rot = seq->rotationLibrary[extIdx];
                block->rotation.type = 1;
                block->rotation.rotQuaternion[0] = rot[0];
                block->rotation.rotQuaternion[1] = rot[1];
                block->rotation.rotQuaternion[2] = rot[2];
                block->rotation.rotQuaternion[3] = rot[3];
                break;
            case EXT_LABELSET:
                labelID = seq->labelsetLibrary[extIdx][0];
                labelValue = seq->labelsetLibrary[extIdx][1];
                block->labelset.type = 1;
                switch (labelID) {
                    case SLC: block->labelset.slc = labelValue; break;
                    case SEG: block->labelset.seg = labelValue; break;
                    case REP: block->labelset.rep = labelValue; break;
                    case AVG: block->labelset.avg = labelValue; break;
                    case SET: block->labelset.set = labelValue; break;
                    case ECO: block->labelset.eco = labelValue; break;
                    case PHS: block->labelset.phs = labelValue; break;
                    case LIN: block->labelset.lin = labelValue; break;
                    case PAR: block->labelset.par = labelValue; break;
                    case ACQ: block->labelset.acq = labelValue; break;
                    case TRID:block->labelset.trid = labelValue; break;
                    case NAV: block->labelset.nav = labelValue; break;
                    case REV: block->labelset.rev = labelValue; break;
                    case SMS: block->labelset.sms = labelValue; break;
                    case REF: block->labelset.ref = labelValue; break;
                    case IMA: block->labelset.ima = labelValue; break;
                    case NOISE: block->labelset.noise = labelValue; break;
                    case PMC: block->labelset.pmc = labelValue; break;
                    case NOROT: block->labelset.norot = labelValue; break;
                    case NOPOS: block->labelset.nopos = labelValue; break;
                    case NOSCL: block->labelset.noscl = labelValue; break;
                    case ONCE: block->labelset.once = labelValue; break;
                    default: break;
                }
                break;
            case EXT_LABELINC:
                labelID = seq->labelincLibrary[extIdx][0];
                labelValue = seq->labelsetLibrary[extIdx][1];
                block->labelinc.type = 1;
                switch (labelID) {
                    case SLC: block->labelinc.slc = labelValue; break;
                    case SEG: block->labelinc.seg = labelValue; break;
                    case REP: block->labelinc.rep = labelValue; break;
                    case AVG: block->labelinc.avg = labelValue; break;
                    case SET: block->labelinc.set = labelValue; break;
                    case ECO: block->labelinc.eco = labelValue; break;
                    case PHS: block->labelinc.phs = labelValue; break;
                    case LIN: block->labelinc.lin = labelValue; break;
                    case PAR: block->labelinc.par = labelValue; break;
                    case ACQ: block->labelinc.acq = labelValue; break;
                    case TRID: block->labelinc.trid = labelValue; break;
                    case NAV: block->labelinc.nav = labelValue; break;
                    case REV: block->labelinc.rev = labelValue; break;
                    case SMS: block->labelinc.sms = labelValue; break;
                    case REF: block->labelinc.ref = labelValue; break;
                    case IMA: block->labelinc.ima = labelValue; break;
                    case NOISE: block->labelinc.noise = labelValue; break;
                    case PMC: block->labelinc.pmc = labelValue; break;
                    case NOROT: block->labelinc.norot = labelValue; break;
                    case NOPOS: block->labelinc.nopos = labelValue; break;
                    case NOSCL: block->labelinc.noscl = labelValue; break;
                    case ONCE: block->labelinc.once = labelValue; break;
                    default: break;
                }
                break;
            case EXT_RF_SHIM:
                rfshim = seq->rfShimLibrary[extIdx];
                block->rfShimming.type = 1;
                block->rfShimming.nChan = rfshim.nChannels;
                block->rfShimming.amplitudes = ALLOC(sizeof(float) * rfshim.nChannels);
                block->rfShimming.phases = ALLOC(sizeof(float) * rfshim.nChannels);
                for (idx = 0; idx < rfshim.nChannels; idx++) {
                    block->rfShimming.amplitudes[idx] = rfshim.values[2 * idx];
                    block->rfShimming.phases[idx] = rfshim.values[2 * idx + 1];
                }
                break;
            case EXT_DELAY:
                delay = seq->softDelayLibrary[extIdx];
                block->delay.type = 1;
                block->delay.numID = delay[0];
                block->delay.offset = delay[1];
                block->delay.factor = delay[2];
                block->delay.hintID = delay[3];
                break;
            default:
                break;
        }
    }

    return block;
}
