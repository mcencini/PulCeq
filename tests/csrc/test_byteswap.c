#include "minunit.h"
#include "pulSeg.h"

#include <stdio.h>

void test_setup(void) {
}

void test_teardown(void) {
}

/* Test byteswap_short */
MU_TEST(test_byteswap_short) {
    short desired = 1;
    short byteswapped = byteswap_short(desired);
    short actual = byteswap_short(byteswapped); /* roundtrip */

    printf("Initial: %d\n", desired);
    printf("Swapped: %d\n", byteswapped);
    printf("Roundtrip: %d\n", actual);

    mu_check(actual == desired);
}

/* Test byteswap_int */
MU_TEST(test_byteswap_int) {
    int desired = 1;
    int byteswapped = byteswap_int(desired);
    int actual = byteswap_int(byteswapped); /* roundtrip */

    printf("Initial: %d\n", desired);
    printf("Swapped: %d\n", byteswapped);
    printf("Roundtrip: %d\n", actual);

    mu_check(actual == desired);
}

/* Test byteswap_float */
MU_TEST(test_byteswap_float) {
    mu_assert_double_eq(1.0f, byteswap_float(byteswap_float(1.0)));
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

