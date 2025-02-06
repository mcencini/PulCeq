/* Toy unit test (remove when we have tests for the actual public reading routines) */
#include <stdint.h>
#include <stdio.h>

#include "minunit.h"
#include "pulSeg.h"

void test_setup(void) {
}

void test_teardown(void) {
}

/* Test byteswap_short */
MU_TEST(test_byteswap_short) {
    short expected = 1;
    short byteswapped = byteswap_short(expected);
    short result = byteswap_short(byteswapped); /* roundtrip */

    printf("Testing short byteswap...\n");
    printf("Initial: %d\n", expected);
    printf("Swapped: %d\n", byteswapped);
    printf("Roundtrip: %d\n", result);
    printf("...done!\n");

    mu_assert_int_eq(expected, result);
}

/* Test byteswap_int */
MU_TEST(test_byteswap_int) {
    int expected = 1;
    int byteswapped = byteswap_int(expected);
    int result = byteswap_int(byteswapped); /* roundtrip */

    printf("Testing int byteswap...\n");
    printf("Initial: %d\n", expected);
    printf("Swapped: %d\n", byteswapped);
    printf("Roundtrip: %d\n", result);
    printf("...done!\n");

    mu_assert_int_eq(expected, result);
}

/* Test byteswap_float */
MU_TEST(test_byteswap_float) {
    float expected = 1.0;
    float byteswapped = byteswap_float(expected);
    float result = byteswap_float(byteswapped); /* roundtrip */

    printf("Testing float byteswap...\n");
    printf("Initial: %.6f\n", expected);
    printf("Swapped: %.6f\n", byteswapped);
    printf("Roundtrip: %.6f\n", result);
    printf("...done!\n");

    mu_assert_double_eq(expected, result);
}


/* Define the test suite */
MU_TEST_SUITE(test_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_byteswap_short);
    MU_RUN_TEST(test_byteswap_int);
    MU_RUN_TEST(test_byteswap_float);
}

/* Main function to run the tests */
int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}

