
#include "minunit.h"
#include "pulseq.h"

MU_TEST(test_seqFile_getBlock_valid) {
    SeqFile* seq;
    SeqBlock* block;
    printf("[DEBUG] Entering test_seqFile_getBlock_valid\n");
    printf("[DEBUG] Calling seqFile...\n");
    seq = seqFile("../../tests/expected_output/seq6.seq");
    printf("[DEBUG] seq pointer: %p\n", (void*)seq);
    mu_assert(seq != NULL, "Failed to open seq6.seq");
    printf("[DEBUG] Calling getBlock(seq, 1, 0)...\n");
    block = getBlock(seq, 1, 0);
    printf("[DEBUG] block pointer: %p\n", (void*)block);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    printf("[DEBUG] Freeing block and seq...\n");
    seqBlockFree(block);
    seqFileFree(seq);
}

MU_TEST(test_seqFile_block_values) {
    SeqFile* seq;
    SeqBlock* block;
    printf("[DEBUG] Entering test_seqFile_block_values\n");
    printf("[DEBUG] Calling seqFile...\n");
    seq = seqFile("../../tests/expected_output/seq6.seq");
    printf("[DEBUG] seq pointer: %p\n", (void*)seq);
    mu_assert(seq != NULL, "Failed to open seq6.seq");
    printf("[DEBUG] Calling getBlock(seq, 1, 0)...\n");
    block = getBlock(seq, 1, 0);
    printf("[DEBUG] block pointer: %p\n", (void*)block);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    printf("[DEBUG] block->rf.type: %d\n", block->rf.type);
    printf("[DEBUG] block->rf.amplitude: %f\n", block->rf.amplitude);
    mu_assert(block->rf.type == 1, "Block 1 should have RF event type 1");
    mu_assert(block->rf.amplitude == 250, "Block 1 RF amplitude should be 250");
    seqBlockFree(block);

    printf("[DEBUG] Calling getBlock(seq, 2, 0)...\n");
    block = getBlock(seq, 2, 0);
    printf("[DEBUG] block pointer: %p\n", (void*)block);
    printf("[DEBUG] block->gx.type: %d\n", block->gx.type);
    printf("[DEBUG] block->gx.amplitude: %f\n", block->gx.amplitude);
    mu_assert(block != NULL, "getBlock should return a valid block for index 1");
    mu_assert(block->gx.type == 1, "Block 2 should have GX event type 1");
    mu_assert(block->gx.amplitude > 1.6e6 && block->gx.amplitude < 1.7e6, "Block 2 GX amplitude should be ~1.69492e6");
    seqBlockFree(block);

    printf("[DEBUG] Calling getBlock(seq, 3, 0)...\n");
    block = getBlock(seq, 3, 0);
    printf("[DEBUG] block pointer: %p\n", (void*)block);
    printf("[DEBUG] block->rotation.type: %d\n", block->rotation.type);
    printf("[DEBUG] block->rotation.rotQuaternion[0]: %f\n", block->rotation.rotQuaternion[0]);
    mu_assert(block != NULL, "getBlock should return a valid block for index 3");
    mu_assert(block->rotation.type == 1, "Block 3 should have rotation extension type 1");
    mu_assert(block->rotation.rotQuaternion[0] > 0.92 && block->rotation.rotQuaternion[0] < 0.93, "Block 3 rotation quaternion[0] should be ~0.92388");
    seqBlockFree(block);

    printf("[DEBUG] Freeing seq...\n");
    seqFileFree(seq);
}

MU_TEST_SUITE(test_seqfile_suite) {
    MU_RUN_TEST(test_seqFile_getBlock_valid);
    MU_RUN_TEST(test_seqFile_block_values);
}

int main(void) {
    MU_RUN_SUITE(test_seqfile_suite);
    MU_REPORT();
    return 0;
}
