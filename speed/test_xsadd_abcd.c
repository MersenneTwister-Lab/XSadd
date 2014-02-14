/**
 * @file test_xsadd_abcd.c
 *
 * @brief XORSHIFT-ADD 128-bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2013 Mutsuo Saito, Makoto Matsumoto and
 * Hiroshima University
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */
#include "xsadd_abcd.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

int main()
{
    xsadd_t xsa;
    xsadd_init(&xsa, 1234);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%10u ", xsadd_uint32(&xsa));
        }
        printf ("\n");
    }
    clock_t start = clock();
    uint32_t sum = 0;
    for (int i = 0; i < 100000000; i++) {
        sum ^= xsadd_uint32(&xsa);
    }
    double ell = clock() - start;
    printf("time = %f\n", ell / CLOCKS_PER_SEC);
    return 0;
}
