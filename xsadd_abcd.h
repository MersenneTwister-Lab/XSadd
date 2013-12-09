#ifndef XSADD_ABCD_H
#define XSADD_ABCD_H
/**
 * @file xsadd_abcd.h
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
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t d;
    } xsadd_t;

    void xsadd_init(xsadd_t * xsadd, uint32_t seed);

    static inline uint32_t xsadd_uint32(xsadd_t * xsadd)
    {
        static const int sh1 = 15;
        static const int sh2 = 18;
        static const int sh3 = 11;
        uint32_t t;
        t = xsadd->a;
        t ^= t << sh1;
        t ^= t >> sh2;
        t ^= xsadd->d << sh3;
        xsadd->a = xsadd->b;
        xsadd->b = xsadd->c;
        xsadd->c = xsadd->d;
        xsadd->d = t;
        return xsadd->d + xsadd->c;
    }

#ifdef __cplusplus
}
#endif

#endif // XSADD_ABCD_H
