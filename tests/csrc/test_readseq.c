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

    /* Cpmplex RF */
    block = getBlock(seq, 1, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 1, "Block 1 should have RF event");
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
    block = getBlock(seq, 1, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->adc.type == 1, "Block 1 should have ADC event");
    mu_assert(block->adc.numSamples > 0, "ADC should have samples");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_grad) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq2.seq");

    /* Gx */
    block = getBlock(seq, 2, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->gx.type != 0, "Block 2 should have GX event");
    seqBlockFree(block);

    /* Gy */
    block = getBlock(seq, 3, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->gy.type != 0, "Block 3 should have GY event");
    seqBlockFree(block);

    /* Gz */
    block = getBlock(seq, 4, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->gz.type != 0, "Block 4 should have GZ event");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_labels) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq2.seq");

    /* LABELSET */
    block = getBlock(seq, 5, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->labelset.type == 1, "Block 5 should have labelset event");
    mu_assert(block->labelset.slc != 0, "Block 5 labelset.slc should be set");
    seqBlockFree(block);

    /* LABELINC */
    block = getBlock(seq, 6, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->labelinc.type == 1, "Block 6 should have labelinc event");
    mu_assert(block->labelinc.seg != 0, "Block 6 labelinc.seg should be set");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST_SUITE(test_seqfile_suite) {
    MU_RUN_TEST(test_basic);
    MU_RUN_TEST(test_rf);
    MU_RUN_TEST(test_adc);
    MU_RUN_TEST(test_grad);
    MU_RUN_TEST(test_labels);
}

int main(void) {
    MU_RUN_SUITE(test_seqfile_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
