#include "minunit.h"

#include "pulseq.h"

MU_TEST(test_seqFile_getBlock_valid) {
    SeqFile* seq;
    SeqBlock* block;
    char cwd[1024];
    char seq_path[1024];

    printf("[DEBUG] Entering test_seqFile_getBlock_valid\n");
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        snprintf(seq_path, sizeof(seq_path), "%s/%s", cwd, "tests/expected_output/seq1.seq");
    }
    seq = seqFile(seq_path);
    readSeq(seq);
    mu_assert(seq != NULL, "Failed to open seq1.seq");
    block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    seqBlockFree(block);
    seqFileFree(seq);
}

/*
MU_TEST(test_seqFile_block_values) {
    SeqFile* seq;
    SeqBlock* block;
    char cwd[1024];
    char seq_path[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        snprintf(seq_path, sizeof(seq_path), "%s/%s", cwd, "tests/expected_output/seq6.seq");
    }

    printf("[DEBUG] Entering test_seqFile_block_values\n");
    seq = seqFile(seq_path);
    readSeq(seq);
    mu_assert(seq != NULL, "Failed to open seq6.seq");
    block = getBlock(seq, 0, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 0");
    mu_assert(block->rf.type == 1, "Block 1 should have RF event type 1");
    mu_assert(block->rf.amplitude == 250, "Block 1 RF amplitude should be 250");
    seqBlockFree(block);
    block = getBlock(seq, 1, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 1");
    mu_assert(block->gx.type == 1, "Block 2 should have GX event type 1");
    mu_assert(block->gx.amplitude > 1.6e6 && block->gx.amplitude < 1.7e6, "Block 2 GX amplitude should be ~1.69492e6");
    seqBlockFree(block);
    block = getBlock(seq, 2, 1);
    mu_assert(block != NULL, "getBlock should return a valid block for index 3");
    mu_assert(block->rotation.type == 1, "Block 3 should have rotation extension type 1");
    mu_assert(block->rotation.rotQuaternion[0] > 0.92 && block->rotation.rotQuaternion[0] < 0.93, "Block 3 rotation quaternion[0] should be ~0.92388");
    seqBlockFree(block);
    seqFileFree(seq);
}
*/

MU_TEST_SUITE(test_seqfile_suite) {
    MU_RUN_TEST(test_seqFile_getBlock_valid);
    /* MU_RUN_TEST(test_seqFile_block_values); */
}

int main(void) {
    MU_RUN_SUITE(test_seqfile_suite);
    MU_REPORT();
    return 0;
}
