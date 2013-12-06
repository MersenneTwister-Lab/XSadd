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
        int index;
    } xsadd_t;

    void xsadd_init(xsadd_t * xsadd, uint32_t seed);

    static inline uint32_t xsadd_uint32(xsadd_t * xsadd)
    {
#define MASK 3
#define sh1 15
#define sh2 18
#define sh3 11
        uint32_t t;
        xsadd->index = (xsadd->index + 1) & MASK;
        t = xsadd->state[xsadd->index];
        t ^= t << sh1;
        t ^= t >> sh2;
        t ^= xsadd->state[(xsadd->index + 3) & MASK] << sh3;
        xsadd->state[xsadd->index & MASK] = t;
        return xsadd->state[xsadd->index & MASK]
            + xsadd->state[(xsadd->index + 3) & MASK];
#undef MASK
#undef sh1
#undef sh2
#undef sh3
    }

#ifdef __cplusplus
}
#endif

#endif // XSADD_H
