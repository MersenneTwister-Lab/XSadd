#ifndef XSADD_SMALL_H
#define XSADD_SMALL_H
/**
 * @file xsadd.h
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

#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        uint32_t state[4];
    } xsadd_t;

    void xsadd_init(xsadd_t * xsadd, uint32_t seed);

    static inline uint32_t xsadd_uint32(xsadd_t * xsadd)
    {
#define sh1 15
#define sh2 18
#define sh3 11
        uint32_t t;
        t = xsadd->state[0];
        t ^= t << sh1;
        t ^= t >> sh2;
        t ^= xsadd->state[3] << sh3;
        xsadd->state[0] = xsadd->state[1];
        xsadd->state[1] = xsadd->state[2];
        xsadd->state[2] = xsadd->state[3];
        xsadd->state[3] = t;
        return xsadd->state[3] + xsadd->state[2];
#undef sh1
#undef sh2
#undef sh3
    }

#ifdef __cplusplus
}
#endif

#endif // XSADD_SMALL_H
