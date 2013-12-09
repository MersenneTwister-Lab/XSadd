#include "xsadd.h"
#define LOOP 8

static void period_certification(xsadd_t * xsadd)
{
    if (xsadd->state[0] == 0 &&
        xsadd->state[0] == 0 &&
        xsadd->state[0] == 0 &&
        xsadd->state[0] == 0) {
        xsadd->state[0] = 'X';
        xsadd->state[1] = 'S';
        xsadd->state[2] = 'A';
        xsadd->state[3] = 'D';
    }
}

void xsadd_init(xsadd_t * xsadd, uint32_t seed)
{
    xsadd->state[0] = seed;
    xsadd->state[1] = 0;
    xsadd->state[2] = 0;
    xsadd->state[3] = 0;
    for (int i = 1; i < LOOP; i++) {
        xsadd->state[i & 3] ^= i + UINT32_C(1812433253)
            * (xsadd->state[(i - 1) & 3]
               ^ (xsadd->state[(i - 1) & 3] >> 30));
    }
    period_certification(xsadd);
    for (int i = 0; i < LOOP; i++) {
        xsadd_uint32(xsadd);
    }
}

