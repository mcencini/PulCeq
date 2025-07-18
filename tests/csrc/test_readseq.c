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

    printf("Entering test_basic\n");
    block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_rf) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq2.seq");

    printf("Entering test_rf\n");
    block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block");
    mu_assert(block->rf.type == 1, "Block 0 should have RF event");
    mu_assert(block->rf.amplitude > 0, "RF amplitude should be positive");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_adc) {
    SeqBlock* block;
    SeqFile* seq = load_seq("tests/expected_output/seq2.seq");

    printf("Entering test_adc\n");
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

    printf("Entering test_grad\n");
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

    printf("Entering test_labels\n");
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
    return 0;
}
