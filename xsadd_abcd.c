#include "xsadd_abcd.h"
#define LOOP 8

static void period_certification(xsadd_t * xsadd)
{
    if (xsadd->a == 0 &&
        xsadd->b == 0 &&
        xsadd->c == 0 &&
        xsadd->d == 0) {
        xsadd->a = 'X';
        xsadd->b = 'S';
        xsadd->c = 'A';
        xsadd->d = 'D';
    }
}

void xsadd_init(xsadd_t * xsadd, uint32_t seed)
{
    uint32_t state[4];
    state[0] = seed;
    state[1] = 0;
    state[2] = 0;
    state[3] = 0;
    for (int i = 1; i < LOOP; i++) {
        state[i & 3] ^= i + UINT32_C(1812433253)
            * (state[(i - 1) & 3]
               ^ (state[(i - 1) & 3] >> 30));
    }
    xsadd->a = state[0];
    xsadd->b = state[1];
    xsadd->c = state[2];
    xsadd->d = state[3];
    period_certification(xsadd);
    for (int i = 0; i < LOOP; i++) {
        xsadd_uint32(xsadd);
    }
}

