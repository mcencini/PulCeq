#include "minunit.h"
#include "pulSeg.h"

#include <stdio.h>
#include <stdint.h>
#include <float.h>

void test_setup(void) {
}

void test_teardown(void) {
}

/* Test byteswap_short */
MU_TEST(test_byteswap_short) {
    mu_assert_int_eq(1, byteswap_short(256));
    mu_assert_int_eq(65280, byteswap_short(255));
}

/* Test byteswap_int */
MU_TEST(test_byteswap_int) {
    mu_assert_int_eq(0x78563412, byteswap_int(0x12345678));
    mu_assert_int_eq(0xAABBCCDD, byteswap_int(0xDDCCBBAA));
    mu_assert_int_eq(0x00000001, byteswap_int(0x01000000));
    mu_assert_int_eq(0xFFFFFFFF, byteswap_int(0xFFFFFFFF));
}

/* Test byteswap_float */
MU_TEST(test_byteswap_float) {
    union { float f; uint32_t i; } input, expected, result;

    input.f = 1.0f;
    expected.i = 0x3F800000;
    expected.i = ((expected.i >> 24) & 0x000000FF) |
                 ((expected.i >> 8)  & 0x0000FF00) |
                 ((expected.i << 8)  & 0x00FF0000) |
                 ((expected.i << 24) & 0xFF000000);

    result.f = byteswap_float(input.f);
    mu_assert_int_eq(expected.i, *(uint32_t*)&result.f);
}


/* Define the test suite */
MU_TEST_SUITE(test_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_byteswap_short);
    MU_RUN_TEST(test_byteswap_int);
    /* MU_RUN_TEST(test_byteswap_float); */
}

/* Main function to run the tests */
int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}

