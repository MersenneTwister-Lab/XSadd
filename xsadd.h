#ifndef XSADD_H
#define XSADD_H
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
        static const int sh1 = 15;
        static const int sh2 = 18;
        static const int sh3 = 11;
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
    }

#ifdef __cplusplus
}
#endif

#endif // XSADD_H
