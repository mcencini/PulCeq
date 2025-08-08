/**
 * @file block.c
 * @brief Implementation of Pulseq block definition and handling Pulseq blocks.
 *
 */

#include <math.h>
#include <stddef.h>
#include <string.h>

#include "seqfile.h"

#include "alloc.h"
#include "block.h"
#include "config.h"
#include "constants.h"

void getRawBlockContentIDs(RawBlock* block, const SeqFile* seq, const int blockIndex, const int parseExtensions)
{
    int i, nextExtID;
    float* eventFloat;
    float* extData;
    int extCount;

    /* Initialize */
    block->adc = 0;
    block->rf = 0;
    block->gx = 0;
    block->gy = 0;
    block->gz = 0;
    block->adc = 0;
    block->extCount = 0;

    /* Sanity check */
    if (seq == 0 || blockIndex < 0 || blockIndex >= seq->numBlocks) {
        return;
    }

    /* Access float data row and cast entries to int */
    eventFloat = seq->blockLibrary[blockIndex];

    int duration = (int)(eventFloat[0]);
    int rfID = (int)(eventFloat[1]) - 1;
    int gxID = (int)(eventFloat[2]) - 1;
    int gyID = (int)(eventFloat[3]) - 1;
    int gzID = (int)(eventFloat[4]) - 1;
    int adcID  = (int)(eventFloat[5]) - 1;
    int extID = (int)(eventFloat[6]);

    block->block_duration = duration;
    block->rf = rfID;
    block->gx = gxID;
    block->gy = gyID;
    block->gz = gzID;
    block->adc = adcID;

    /* Handle extensions if present */
    if (parseExtensions && extID > 0 && seq->isExtensionsLibraryParsed) {
        nextExtID = extID;
        extCount = 0;

        while (nextExtID > 0 && nextExtID <= seq->extensionsLibrarySize) {
            extData = seq->extensionsLibrary[nextExtID - 1]; /* [type, ref, next_id] */
            block->ext[extCount][0] = (int)extData[0];      /* type */
            block->ext[extCount][1] = (int)extData[1] - 1;  /* ref */
            nextExtID = (int)extData[2]; /* next in chain */
            extCount += 1;
        }

        block->extCount = extCount;
    }

    return;
}

int __seqBlock(SeqBlock* block)
{
    /* Event structures used to initialize the block */
    RFEvent rf;
    GradEvent gx;
    GradEvent gy;
    GradEvent gz;
    ADCEvent adc;
    TriggerEvent trigger;
    RotationEvent rotation;
    FlagEvent flag;
    LabelEvent label;
    SoftDelayEvent delay;
    RfShimmingEvent rfShimming;
    
    /* Check for null pointer */
    if (!block) return 0;

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

    /* Initialize flag values to 0 */
    flag.trid = 0;
    flag.nav = 0;
    flag.rev = 0;
    flag.sms = 0;
    flag.ref = 0;
    flag.ima = 0;
    flag.noise = 0;
    flag.pmc = 0;
    flag.norot = 0;
    flag.nopos = 0;
    flag.noscl = 0;
    flag.once = 0;
    
    /* Initialize label values to 0 */
    label.slc = 0;
    label.seg = 0;
    label.rep = 0;
    label.avg = 0;
    label.set = 0;
    label.eco = 0;
    label.phs = 0;
    label.lin = 0;
    label.par = 0;
    label.acq = 0;

    /* Initialize the block */
    block->rf = rf;
    block->gx = gx;
    block->gy = gy;
    block->gz = gz;
    block->adc = adc;
    block->trigger = trigger;
    block->rotation = rotation;
    block->flag = flag;
    block->label = label;
    block->delay = delay;
    block->rfShimming = rfShimming;

    return 1;
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


int __getBlock(const SeqFile* seq, int blockIndex, int parseExtensions, SeqBlock* block) {
    float* farray;
    int idx;
    int i, labelID, labelValue, extType, extIdx;
    int numRealSamples = 0;
    float* trig;
    float* rot;
    float* delay;
    int* isRealSample;
    RfShimEntry rfshim;
    RawBlock rawBlock;
    ShapeArbitrary shape;
    
    /* Check inputs */
    if (!seq || !block || blockIndex < 0 || blockIndex >= seq->numBlocks) {
        return 0; /* Invalid inputs */
    }
    
    getRawBlockContentIDs(&rawBlock, seq, blockIndex, parseExtensions);

    /* Set the duration */
    block->duration = rawBlock.block_duration;

    /* ------------------ RF Event ------------------ */
    if (rawBlock.rf >= 0) {
        farray = seq->rfLibrary[rawBlock.rf];
        block->rf.type = 1;
        block->rf.amplitude = farray[0];

        idx = (int)farray[1];
        if (idx > 0) {
            if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                return 0; /* Failed to decompress shape */
            }
            block->rf.magShape = shape;
        }

        idx = (int)farray[2];
        if (idx > 0) {
            if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                return 0; /* Failed to decompress shape */
            }
            block->rf.phaseShape = shape;
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

            /* If all samples are real, restore sign, set the phase shape to 0 samples and free it */
            if (numRealSamples == block->rf.magShape.numSamples) {

                /* Restore sign of magnitude shape */
                for (i = 0; i < block->rf.magShape.numSamples; i++) {
                    if (fabs(block->rf.phaseShape.samples[i] - M_PI) < 1e-6) {
                        block->rf.magShape.samples[i] *= -1;
                    }
                }

                /* Free phase shape */
                block->rf.phaseShape.numSamples = 0; /* Set phase shape to 0 samples */
                block->rf.phaseShape.numUncompressedSamples = 0; /* Set phase shape to 0 samples */
                FREE(block->rf.phaseShape.samples); /* Free phase shape samples */
                block->rf.phaseShape.samples = NULL; /* Free phase shape samples */
            }
            FREE(isRealSample);
        }

        idx = (int)farray[3];
        if (idx > 0) {
            if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                return 0; /* Failed to decompress shape */
            }
            block->rf.timeShape = shape;
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
            if (idx > 0) {
                if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                    return 0; /* Failed to decompress shape */
                }
                block->gx.waveShape = shape;
            }

            idx = (int)farray[5];
            if (idx > 0) {
                if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                    return 0; /* Failed to decompress shape */
                }
                block->gx.timeShape = shape;
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
            if (idx > 0) {
                if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                    return 0; /* Failed to decompress shape */
                }
                block->gy.waveShape = shape;
            }

            idx = (int)farray[5];
            if (idx > 0) {
                if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                    return 0; /* Failed to decompress shape */
                }
                block->gy.timeShape = shape;
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
            if (idx > 0) {
                if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                    return 0; /* Failed to decompress shape */
                }
                block->gz.waveShape = shape;
            }

            idx = (int)farray[5];
            if (idx > 0) {
                if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                    return 0; /* Failed to decompress shape */
                }
                block->gz.timeShape = shape;
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

        /* Load ADC labels from labelMap if available and compatible */
        if (seq->areLabelsCompatible) {
            /* Try to get labels from the sparse map */
            if (!getLabelsByAdcIndex((SeqFile*)seq, rawBlock.adc, &block->label)) {
                /* If not found in sparse map, initialize to 0 */
                memset(&block->label, 0, sizeof(LabelEvent));
            }
        }

        idx = (int)farray[7];
        if (idx > 0) {
            if (!decompressShape(&(seq->shapesLibrary[idx - 1]), &shape)) {
                return 0; /* Failed to decompress shape */
            }
            block->adc.phaseModulationShape = shape;
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
                block->trigger.duration = (long)trig[3];
                block->trigger.delay = (long)trig[2];
                block->trigger.triggerType = (int)trig[0];
                block->trigger.triggerChannel = (int)trig[1];
                break;
            case EXT_ROTATION:
                {
                    int ridx;
                    block->rotation.type = 1;
                    
                    #if ROTATION_FORMAT == ROTATION_FORMAT_QUATERNION
                    if (seq->rotationQuaternionLibrary) {
                        rot = seq->rotationQuaternionLibrary[extIdx];
                        block->rotation.data.rotQuaternion[0] = rot[0];
                        block->rotation.data.rotQuaternion[1] = rot[1];
                        block->rotation.data.rotQuaternion[2] = rot[2];
                        block->rotation.data.rotQuaternion[3] = rot[3];
                    }
                    #elif ROTATION_FORMAT == ROTATION_FORMAT_MATRIX
                    if (seq->rotationMatrixLibrary) {
                        /* Copy the rotation matrix data */
                        for (ridx = 0; ridx < 9; ridx++) {
                            block->rotation.data.rotMatrix[ridx] = seq->rotationMatrixLibrary[extIdx][ridx];
                        }
                    }
                    #endif
                }
                break;
            case EXT_LABELSET:
                labelID = seq->labelsetLibrary[extIdx][1];
                labelValue = seq->labelsetLibrary[extIdx][0];
                
                /* Handle flag values - those that don't affect ADC labeling */
                switch (labelID) {
                    case TRID: block->flag.trid = labelValue; break;
                    case NAV: block->flag.nav = labelValue; break;
                    case REV: block->flag.rev = labelValue; break;
                    case SMS: block->flag.sms = labelValue; break;
                    case REF: block->flag.ref = labelValue; break;
                    case IMA: block->flag.ima = labelValue; break;
                    case NOISE: block->flag.noise = labelValue; break;
                    case PMC: block->flag.pmc = labelValue; break;
                    case NOROT: block->flag.norot = labelValue; break;
                    case NOPOS: block->flag.nopos = labelValue; break;
                    case NOSCL: block->flag.noscl = labelValue; break;
                    case ONCE: block->flag.once = labelValue; break;
                    default: break;
                }
                break;
            case EXT_LABELINC:
                /* Label increment values are processed when building labelLibrary,
                   but we don't need to store them in the block itself */
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

    return 1; /* Success */
}
