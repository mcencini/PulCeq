#include "minunit.h"

#include "pulseq.h"

static SeqFile* load_seq(char* filePath) {
    char cwd[1024];
    char seq_path[1024];
    SeqFile* seq;
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(seq_path, sizeof(seq_path), "%s/%s", cwd, filePath);
    }
    seq = seqFile(seq_path);
    readSeq(seq);
    return seq;
}

MU_TEST(test_basic) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq1.seq");
    block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_rf) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq2.seq");

    /* Real RF */
    block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 1, "Block 0 should have RF event");
    mu_assert(block->gx.type == 0, "Block 0 should not have Gx event");
    mu_assert(block->gy.type == 0, "Block 0 should not have Gy event");
    mu_assert(block->gz.type == 0, "Block 0 should not have Gz event");
    mu_assert(block->adc.type == 0, "Block 0 should not have ADC event");
    mu_assert(block->rotation.type == 0, "Block 0 should not have rotation event");

    mu_assert(block->rf.amplitude > 0, "RF amplitude should be positive");
    mu_assert(block->rf.magShape.numSamples > 0, "RF should have magnitude shape samples");
    mu_assert(block->rf.timeShape.numSamples == 0, "RF should not have time shape samples");
    mu_assert(block->rf.phaseShape.numSamples == 0, "RF should not have phase shape samples");
    mu_assert(fabs(block->rf.phaseOffset) < 1e-6, "RF should not have phase offset");
    mu_assert(fabs(block->rf.freqOffset) < 1e-6, "RF should not have freq offset");
    mu_assert(fabs(block->rf.phasePPM) < 1e-6, "RF should not have PPM phase offset");
    mu_assert(fabs(block->rf.freqPPM) < 1e-6, "RF should not have PPM freq offset");
    mu_assert(block->rf.delay == 0, "RF should not have delay");
    mu_assert(fabs(block->rf.center - 2000.0) < 1e-6, "RF should have center at 2000 us");

    /* Complex RF */
    block = getBlock(seq, 1, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 1, "Block 1 should have RF event");
    mu_assert(block->gx.type == 0, "Block 1 should not have Gx event");
    mu_assert(block->gy.type == 0, "Block 1 should not have Gy event");
    mu_assert(block->gz.type == 0, "Block 1 should not have Gz event");
    mu_assert(block->adc.type == 0, "Block 1 should not have ADC event");
    mu_assert(block->rotation.type == 0, "Block 1 should not have rotation event");

    mu_assert(block->rf.amplitude > 0, "RF amplitude should be positive");
    mu_assert(block->rf.magShape.numSamples > 0, "RF should have magnitude shape samples");
    mu_assert(block->rf.timeShape.numSamples == 0, "RF should not have time shape samples");
    mu_assert(block->rf.phaseShape.numSamples > 0, "RF should have phase shape samples");
    mu_assert(fabs(block->rf.phaseOffset) < 1e-6, "RF should not have phase offset");
    mu_assert(fabs(block->rf.freqOffset) < 1e-6, "RF should not have freq offset");
    mu_assert(fabs(block->rf.phasePPM) < 1e-6, "RF should not have PPM phase offset");
    mu_assert(fabs(block->rf.freqPPM) < 1e-6, "RF should not have PPM freq offset");
    mu_assert(block->rf.delay == 0, "RF should not have delay");
    mu_assert(fabs(block->rf.center - 5000.5) < 1e-6, "RF should have center at 5000.5 us");

    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_adc) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq2.seq");

    /* phase modulated ADC */
    block = getBlock(seq, 2, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 2 should not have RF event");
    mu_assert(block->gx.type == 0, "Block 2 should not have Gx event");
    mu_assert(block->gy.type == 0, "Block 2 should not have Gy event");
    mu_assert(block->gz.type == 0, "Block 2 should not have Gz event");
    mu_assert(block->adc.type == 1, "Block 2 should have ADC event");
    mu_assert(block->rotation.type == 0, "Block 2 should not have rotation event");

    mu_assert(block->adc.numSamples == 128, "ADC should have 128 samples");
    mu_assert(block->adc.dwellTime == 10000, "ADC should have 10us dwell time");
    mu_assert(fabs(block->adc.phaseOffset) < 1e-6, "ADC should not have phase offset");
    mu_assert(fabs(block->adc.freqOffset) < 1e-6, "ADC should not have freq offset");
    mu_assert(fabs(block->adc.phasePPM) < 1e-6, "ADC should not have PPM phase offset");
    mu_assert(fabs(block->adc.freqPPM) < 1e-6, "ADC should not have PPM freq offset");
    mu_assert(block->adc.delay == 0, "ADC should not have delay");
    mu_assert(block->adc.phaseModulationShape.numSamples > 0, "ADC should have phase modulation");

    /* standard ADC */
    block = getBlock(seq, 3, 0); /* do not parse extensions here */
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 3 should not have RF event");
    mu_assert(block->gx.type == 0, "Block 3 should not have Gx event");
    mu_assert(block->gy.type == 0, "Block 3 should not have Gy event");
    mu_assert(block->gz.type == 0, "Block 3 should not have Gz event");
    mu_assert(block->adc.type == 1, "Block 3 should have ADC event");
    mu_assert(block->rotation.type == 0, "Block 3 should not have rotation event");

    mu_assert(block->adc.numSamples == 128, "ADC should have 128 samples");
    mu_assert(block->adc.dwellTime == 10000, "ADC should have 10us dwell time");
    mu_assert(fabs(block->adc.phaseOffset) < 1e-6, "ADC should not have phase offset");
    mu_assert(fabs(block->adc.freqOffset) < 1e-6, "ADC should not have freq offset");
    mu_assert(fabs(block->adc.phasePPM) < 1e-6, "ADC should not have PPM phase offset");
    mu_assert(fabs(block->adc.freqPPM) < 1e-6, "ADC should not have PPM freq offset");
    mu_assert(block->adc.delay == 0, "ADC should not have delay");
    mu_assert(block->adc.phaseModulationShape.numSamples == 0, "ADC should not have phase modulation");

    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_grad) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq2.seq");

    /**** TRAPEZOIDS ****/
    /* Gx */
    block = getBlock(seq, 36, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 36 should not have RF event");
    mu_assert(block->gx.type == 1, "Block 36 should have trapezoidal Gx event");
    mu_assert(block->gy.type == 0, "Block 36 should not have Gy event");
    mu_assert(block->gz.type == 0, "Block 36 should not have Gz event");
    mu_assert(block->adc.type == 0, "Block 36 should not have ADC event");
    mu_assert(block->rotation.type == 0, "Block 36 should not have rotation event");

    mu_assert(fabs(block->gx.amplitude - 10) < 1e-6, "Gx amplitude should be 10 Hz/m");
    mu_assert(block->gx.delay == 0, "Gx should not have delay");
    mu_assert(block->gx.trap.riseTime == 10, "Gx rise time should be 10 us");
    mu_assert(block->gx.trap.flatTime == 980, "Gx flat time should be 980 us");
    mu_assert(block->gx.trap.fallTime == 10, "Gx fall time should be  10 us");
    mu_assert(block->gx.first == 0, "Gx first sample should be 0");
    mu_assert(block->gx.last == 0, "Gx last sample should be 0");

    /* Gy */
    block = getBlock(seq, 37, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 37 should not have RF event");
    mu_assert(block->gx.type == 0, "Block 37 should not have Gx event");
    mu_assert(block->gy.type == 1, "Block 37 should have trapezoidal Gy event");
    mu_assert(block->gz.type == 0, "Block 37 should not have Gz event");
    mu_assert(block->adc.type == 0, "Block 37 should not have ADC event");
    mu_assert(block->rotation.type == 0, "Block 37 should not have rotation event");

    mu_assert(fabs(block->gy.amplitude - 5154.64014) < 1e-5, "Gy amplitude should be 5154.64 Hz/m");
    mu_assert(block->gy.delay == 0, "Gy should not have delay");
    mu_assert(block->gy.trap.riseTime == 30, "Gy rise time should be 30 us");
    mu_assert(block->gy.trap.flatTime == 940, "Gy flat time should be 940 us");
    mu_assert(block->gy.trap.fallTime == 30, "Gy fall time should be  30 us");
    mu_assert(block->gy.first == 0, "Gy first sample should be 0");
    mu_assert(block->gy.last == 0, "Gy last sample should be 0");

    /* Gz */
    block = getBlock(seq, 38, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 38 should not have RF event");
    mu_assert(block->gx.type == 0, "Block 38 should not have Gx event");
    mu_assert(block->gy.type == 0, "Block 38 should not have Gy event");
    mu_assert(block->gz.type == 1, "Block 38 should have trapezoidal Gz event");
    mu_assert(block->adc.type == 0, "Block 38 should not have ADC event");
    mu_assert(block->rotation.type == 0, "Block 38 should not have rotation event");

    mu_assert(fabs(block->gz.amplitude - 5154.64014) < 1e-5, "Gz amplitude should be 5154.64 Hz/m");
    mu_assert(block->gz.delay == 0, "Gz should not have delay");
    mu_assert(block->gz.trap.riseTime == 30, "Gz rise time should be 30 us");
    mu_assert(block->gz.trap.flatTime == 940, "Gz flat time should be 940 us");
    mu_assert(block->gz.trap.fallTime == 30, "Gz fall time should be  30 us");
    mu_assert(block->gz.first == 0, "Gz first sample should be 0");
    mu_assert(block->gz.last == 0, "Gz last sample should be 0");
    /**** END TRAPEZOIDS ****/

    /**** ARBITRARY GRAD ****/
    /* Gx */
    block = getBlock(seq, 39, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 39 should not have RF event");
    mu_assert(block->gx.type == 2, "Block 39 should have arbitrary Gx event");
    mu_assert(block->gy.type == 0, "Block 39 should not have Gy event");
    mu_assert(block->gz.type == 0, "Block 39 should not have Gz event");
    mu_assert(block->adc.type == 0, "Block 39 should not have ADC event");
    mu_assert(block->rotation.type == 1, "Block 39 should have rotation event");

    mu_assert(fabs(block->gx.amplitude - 1) < 1e-6, "Gx amplitude should be 1 Hz/m");
    mu_assert(block->gx.delay == 0, "Gx should not have delay");
    mu_assert(block->gx.waveShape.numSamples > 0, "Gx should have magnitude shape samples");
    mu_assert(block->gx.timeShape.numSamples == 0, "Gx should not have time shape samples");
    mu_assert(fabs(block->gx.first + 0.05), "Gx first sample should be -0.05");
    mu_assert(fabs(block->gx.last + 0.05), "Gx last sample should be -0.05");
    printf("quat0 = %.10f\n", block->rotation.rotQuaternion[0]);
    mu_assert(fabs(block->rotation.rotQuaternion[0] - 1.0) < 1e-6, "Rotation quaternion quat0 should be 1.0");
    mu_assert(fabs(block->rotation.rotQuaternion[1]) < 1e-6, "Rotation quaternion quatX should be 0.0");
    mu_assert(fabs(block->rotation.rotQuaternion[2]) < 1e-6, "Rotation quaternion quatY should be 0.0");
    mu_assert(fabs(block->rotation.rotQuaternion[3]) < 1e-6, "Rotation quaternion quatZ should be 0.0");

    /* Gy */
    block = getBlock(seq, 40, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 40 should not have RF event");
    mu_assert(block->gx.type == 0, "Block 40 should not have Gx event");
    mu_assert(block->gy.type == 2, "Block 40 should have arbitrary Gy event");
    mu_assert(block->gz.type == 0, "Block 40 should not have Gz event");
    mu_assert(block->adc.type == 0, "Block 40 should not have ADC event");
    mu_assert(block->rotation.type == 1, "Block 40 should have rotation event");

    mu_assert(fabs(block->gy.amplitude - 1) < 1e-6, "Gy amplitude should be 1 Hz/m");
    mu_assert(block->gy.delay == 0, "Gy should not have delay");
    mu_assert(block->gy.waveShape.numSamples > 0, "Gy should have magnitude shape samples");
    mu_assert(block->gy.timeShape.numSamples == 0, "Gy should not have time shape samples");
    mu_assert(fabs(block->gy.first + 0.05), "Gy first sample should be -0.05");
    mu_assert(fabs(block->gy.last + 0.05), "Gy last sample should be -0.05");
    mu_assert(fabs(block->rotation.rotQuaternion[0] - 1.0) < 1e-6, "Rotation quaternion quat0 should be 1.0");
    mu_assert(fabs(block->rotation.rotQuaternion[1]) < 1e-6, "Rotation quaternion quatX should be 0.0");
    mu_assert(fabs(block->rotation.rotQuaternion[2]) < 1e-6, "Rotation quaternion quatY should be 0.0");
    mu_assert(fabs(block->rotation.rotQuaternion[3]) < 1e-6, "Rotation quaternion quatZ should be 0.0");

    /* Gz */
    block = getBlock(seq, 41, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 0, "Block 41 should not have RF event");
    mu_assert(block->gx.type == 0, "Block 41 should not have Gx event");
    mu_assert(block->gy.type == 0, "Block 41 should not have Gy event");
    mu_assert(block->gz.type == 2, "Block 41 should have arbitrary Gz event");
    mu_assert(block->adc.type == 0, "Block 41 should not have ADC event");
    mu_assert(block->rotation.type == 1, "Block 41 should have rotation event");

    mu_assert(fabs(block->gz.amplitude - 1) < 1e-6, "Gz amplitude should be 1 Hz/m");
    mu_assert(block->gz.delay == 0, "Gz should not have delay");
    mu_assert(block->gz.waveShape.numSamples > 0, "Gz should have magnitude shape samples");
    mu_assert(block->gz.timeShape.numSamples == 0, "Gz should not have time shape samples");
    mu_assert(fabs(block->gz.first + 0.05), "Gz first sample should be -0.05");
    mu_assert(fabs(block->gz.last + 0.05), "Gz last sample should be -0.05");
    mu_assert(fabs(block->rotation.rotQuaternion[0] - 1.0) < 1e-6, "Rotation quaternion quat0 should be 1.0");
    mu_assert(fabs(block->rotation.rotQuaternion[1]) < 1e-6, "Rotation quaternion quatX should be 0.0");
    mu_assert(fabs(block->rotation.rotQuaternion[2]) < 1e-6, "Rotation quaternion quatY should be 0.0");
    mu_assert(fabs(block->rotation.rotQuaternion[3]) < 1e-6, "Rotation quaternion quatZ should be 0.0");
    /**** END ARBITRARY GRAD ****/

    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST_SUITE(test_seqfile_suite) {
    MU_RUN_TEST(test_basic);
    MU_RUN_TEST(test_rf);
    MU_RUN_TEST(test_adc);
    MU_RUN_TEST(test_grad);
}

int main(void) {
    MU_RUN_SUITE(test_seqfile_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
