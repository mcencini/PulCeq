
#include "minunit.h"
#include "pulseq.h"

MU_TEST(test_seqFile_getBlock_valid) {
    SeqFile* seq = seqFile("../../tests/expected_output/seq6.seq");
    mu_assert(seq != NULL, "Failed to open seq6.seq");
    SeqBlock* block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_seqFile_getBlock_invalid) {
    SeqFile* seq = seqFile("../../tests/expected_output/seq6.seq");
    mu_assert(seq != NULL, "Failed to open seq6.seq");
    SeqBlock* block = getBlock(seq, -1, 1);
    mu_assert(block != NULL, "getBlock should return a block (even if empty) for invalid index");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_seqFile_block_values) {
    SeqFile* seq = seqFile("../../tests/expected_output/seq6.seq");
    mu_assert(seq != NULL, "Failed to open seq6.seq");
    SeqBlock* block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    mu_assert(block->rf.type == 1, "Block 0 should have RF event type 1");
    mu_assert(block->rf.amplitude == 250, "Block 0 RF amplitude should be 250");
    seqBlockFree(block);

    block = getBlock(seq, 1, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 1");
    mu_assert(block->gx.type == 1, "Block 1 should have GX event type 1");
    mu_assert(block->gx.amplitude > 1.6e6 && block->gx.amplitude < 1.7e6, "Block 1 GX amplitude should be ~1.69492e6");
    seqBlockFree(block);

    block = getBlock(seq, 3, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 3");
    mu_assert(block->rotation.type == 1, "Block 3 should have rotation extension type 1");
    mu_assert(block->rotation.rotQuaternion[0] > 0.92 && block->rotation.rotQuaternion[0] < 0.93, "Block 3 rotation quaternion[0] should be ~0.92388");
    seqBlockFree(block);

    seqFileFree(seq);
}

MU_TEST_SUITE(test_seqfile_suite) {
    MU_RUN_TEST(test_seqFile_getBlock_valid);
    MU_RUN_TEST(test_seqFile_getBlock_invalid);
    MU_RUN_TEST(test_seqFile_block_values);
}

int main(void) {
    MU_RUN_SUITE(test_seqfile_suite);
    MU_REPORT();
    return 0;
}
