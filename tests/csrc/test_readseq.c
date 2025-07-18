#include "minunit.h"

#include "pulseq.h"

MU_TEST(test_basic) {
    SeqFile* seq;
    SeqBlock* block;
    char cwd[1024];
    char seq_path[1024];

    printf("Entering test_basic\n");
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

MU_TEST_SUITE(test_seqfile_suite) {
    MU_RUN_TEST(test_basic);
}

int main(void) {
    MU_RUN_SUITE(test_seqfile_suite);
    MU_REPORT();
    return 0;
}
