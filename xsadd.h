#ifndef XSADD_H
#define XSADD_H
/**
 * @file xsadd.h
 *
 * @brief XORSHIFT-ADD: 128-bit internal state pseudorandom number generator.
 *
 * @author Mutsuo Saito
 *
 * Copyright (c) 2014
 * Mutsuo Saito, Makoto Matsumoto, Hiroshima University
 * and Manieth Corp.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
    void xsadd_init_by_array(xsadd_t * xsadd, uint32_t seed[], int size);

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

    /**
     * Recomended large jump step.
     * The value is 3^41.
     */
    extern const char * const xsadd_jump_base_step;

    /**
     * jump function
     * @param xsadd xsadd structure, overwritten by new state after calling
     * this function.
     * @param mul_step jump step is mul_step * base_step.
     * @param base_step hexadecimal number string less than 2<sup>128</sup>.
     * @param poly_str string of the characteristic polynomial.
     */
    void xsadd_jump(xsadd_t * xsadd,
		    uint32_t mul_step,
		    const char * base_step);

    /**
     * jump using the jump polynomial.
     * This function is not as time consuming as calculating jump polynomial.
     * This function can use multiple time for the xsadd structure.
     * @param xsadd xsadd structure, overwritten by new state after calling
     * this function.
     * @param jump_poly the jump polynomial calculated by
     * xsadd_calculate_jump_polynomial.
     */
    void xsadd_jump_by_polynomial(xsadd_t * xsadd, const char * jump_str);

    /**
     * calculate jump polynomial.
     * This function is time consuming.
     * jump_str needs 17 bytes memory.
     *
     * @param[out] jump_str the result of this calculation.
     * @param[in] mul_step jump step is mul_step * base_step.
     * @param[in] base_step hexadecimal string of jump base.
     * @param[in] poly_str string of the characteristic polynomial given by
     * pseudo random number generator. This may a multiple of characteristic
     * polynomial.
     */
    void xsadd_calculate_jump_polynomial(char * jump_str,
					 uint32_t mul_step,
					 const char * base_step);


#ifdef __cplusplus
}
#endif

#endif // XSADD_H
