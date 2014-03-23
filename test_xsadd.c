/**
 * @file test_xsadd.c
 *
 * @brief XORSHIFT-ADD 128-bit internal state
 *
 * @author Mutsuo Saito (Manieth Corp.)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2013 Mutsuo Saito, Makoto Matsumoto and
 * Hiroshima University
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */
#include "xsadd.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>

void speed(void);
void print_output(void);

int main(int argc, char * argv[])
{
    enum {PRINT, SPEED} mode;
    if (argc <= 1) {
	mode = PRINT;
    } else if (strcmp(argv[1], "-s") == 0) {
	mode = SPEED;
    } else {
	printf("%s [-s]\n", argv[0]);
	return 1;
    }
    if (mode == PRINT) {
	print_output();
    } else {
	speed();
    }
    return 0;
}

void print_output()
{
    xsadd_t xsa;
    xsadd_init(&xsa, 1234);
    printf("xsadd_init(&xsa, 1234);\n");
    printf("xsadd_uint32\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%10u ", xsadd_uint32(&xsa));
        }
        printf ("\n");
    }
    printf("xsadd_float\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.8f ", xsadd_float(&xsa));
        }
        printf ("\n");
    }
    printf("xsadd_floatOC\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.8f ", xsadd_floatOC(&xsa));
        }
        printf ("\n");
    }
    uint32_t seed[4] = {0x0a, 0x0b, 0x0c, 0x0d};
    xsadd_init_by_array(&xsa, seed, 4);
    printf("\nuint32_t seed[4] = {0x0a, 0x0b, 0x0c, 0x0d};\n");
    printf("xsadd_init_by_array(&xsa, seed, 4);\n");
    printf("xsadd_uint32\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%08" PRIx32 " ", xsadd_uint32(&xsa));
        }
        printf ("\n");
    }
    printf("xsadd_double\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.15f ", xsadd_double(&xsa));
        }
        printf ("\n");
    }
}

void speed()
{
    xsadd_t xsa;
    xsadd_init(&xsa, 1234);
    clock_t start = clock();
    uint32_t sum = 0;
    for (int i = 0; i < 100000000; i++) {
	sum ^= xsadd_uint32(&xsa);
    }
    double ell = clock() - start;
    printf("consumed time for generating 10^8 numbers = %.2fms\n",
	   (ell / CLOCKS_PER_SEC) * 1000);
}
