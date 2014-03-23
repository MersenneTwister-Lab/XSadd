/**
 * This is a test progam using TestU01:
 * http://www.iro.umontreal.ca/~simardr/testu01/tu01.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include "unif01.h"
#include "bbattery.h"
#include "crushes.h"

static void set_rep(int rep[200], int test[]);
static void set_rep(int rep[200], int test[])
{
    for (int i = 0; i < 200; i++) {
        rep[i] = 0;
    }
    for (int i = 0; i < 200; i++) {
        int n = test[i];
        if (n < 0) {
            break;
        }
        if (n < 200) {
            rep[n] = 1;
        }
    }
}

void crushes(char type, uint32_t seed)
{
    unif01_Gen *gen;
#if 1
    int limited_test = 1;
    int small_test[] = {-1};
/*
    int medium_test[] = {2, 6, 7, 8, 9, 10, 13, 15, 16, 17, 20, 23,
                         24, 26, 27, 28, 30, 32, 35, 36, 37, 38,
                         39, 40, 50, 55, 62, 66, 68, 80, 86, 88, 96,
                         -1};
*/
    //int medium_test[] = {58, 59, 60, 61, 71, 72, -1};
    //int medium_test[] = {71, -1};
    int medium_test[] = {-1};
    int big_test[] = {37, -1};
/*
    int big_test[] = {1, 2, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 18, 19,
                      20, 21, 22, 25, 30, 31, 32, 33, 34, 35, 42, 46,
                      50, 59, 60, 61, 62, 64, 66, 67, 73, 75, 76,
                      77, 79, 82, 86, 96, 97, 98, 102, 105, -1};
*/
    int rep[200] = {0};
#endif
    test_init(seed);

    gen = unif01_CreateExternGenBits(test_name(), test_generator);
    switch (type) {
    case 'S':
    case 's':
        if (limited_test && small_test[0] > 0) {
            set_rep(rep, small_test);
            bbattery_RepeatSmallCrush(gen, rep);
        } else {
            bbattery_SmallCrush(gen);
        }
        break;
    case 'M':
    case 'm':
        if (limited_test && medium_test[0] > 0) {
            set_rep(rep, medium_test);
            bbattery_RepeatCrush(gen, rep);
        } else {
            bbattery_Crush(gen);
        }
        break;
    case 'B':
    case 'b':
    default:
        if (limited_test && big_test[0] > 0) {
            set_rep(rep, big_test);
            bbattery_RepeatBigCrush(gen, rep);
        } else {
            bbattery_BigCrush(gen);
        }
        break;
    }
    unif01_DeleteExternGenBits(gen);
}

#if defined(MAIN)
int main(int argc, char *argv[]) {
    uint32_t seed;
    char test_type;
    errno = 0;
    if (argc <= 2) {
        printf("%s: seed test_type\n", argv[0]);
        printf("\tseed: seed of random\n");
        printf("\ttest_type: small, medium or big\n");
        return 1;
    }
    seed = strtoul(argv[1], NULL, 10);
    test_type = argv[2][0];
    test_init(seed);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%08"PRIx32" ", test_generator());
        }
        printf("\n");
    }
    crushes(test_type, seed);
    return 0;
}
#endif
