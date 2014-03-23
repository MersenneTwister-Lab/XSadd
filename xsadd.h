#ifndef XSADD_H
#define XSADD_H
/**
 * @file xsadd.h
 *
 * @brief XORSHIFT-ADD: 128-bit internal state pseudorandom number generator.
 *
 * @author Mutsuo Saito (Manieth Corp.)
 * @author Makoto Matsumoto (Hiroshima University)
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

#define XSADD_FLOAT_MUL (1.0f / 4294967296.0f)
#define XSADD_DOUBLE_MUL (1.0 / 18446744073709551616.0)

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * xsadd internal state vector and parameters
     */
    typedef struct {
        uint32_t state[4];
    } xsadd_t;

    /**
     * This function initializes the internal state array with a 32-bit
     * unsigned integer seed.
     * @param[out] xsadd xsadd state vector.
     * @param[in] seed a 32-bit unsigned integer used as a seed.
     */
    void xsadd_init(xsadd_t * xsadd, uint32_t seed);

    /**
     * This function initializes the internal state array,
     * with an array of 32-bit unsigned integers used as seeds
     * @param[out] xsadd xsadd state vector.
     * @param[in] seed the array of 32-bit integers, used as a seed.
     * @param[in] size the length of seed.
     */
    void xsadd_init_by_array(xsadd_t * xsadd, const uint32_t seed[], int size);

    /**
     * This function changes internal state of xsadd.
     * Users should not call this function directly.
     * @param[in,out] xsadd xsadd internal state
     */
    static inline void xsadd_next_state(xsadd_t * xsadd)
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
    }

    /**
     * This function outputs 32-bit unsigned integer from internal state.
     * @param[in,out] xsadd xsadd internal state
     * @return 32-bit unsigned integer r (0 <= r < 2^32)
     */
    static inline uint32_t xsadd_uint32(xsadd_t * xsadd)
    {
	xsadd_next_state(xsadd);
        return xsadd->state[3] + xsadd->state[2];
    }

    /**
     * This function outputs floating point number from internal state.
     * This function is implemented using multiplying by 1 / 2^32.
     * floating point multiplication is faster than using union trick in
     * my Intel CPU.
     * @param xsadd xsadd internal state
     * @return floating point number r (0.0 <= r < 1.0)
     */
    inline static float xsadd_float(xsadd_t * xsadd) {
	return xsadd_uint32(xsadd) * XSADD_FLOAT_MUL;
    }

    /**
     * This function outputs floating point number from internal state.
     * This function may return 1.0 and never returns 0.0.
     * @param[in,out] xsadd xsadd internal state
     * @return floating point number r (0.0 < r <= 1.0)
     */
    inline static float xsadd_floatOC(xsadd_t * xsadd) {
	xsadd_next_state(xsadd);
	return 1.0f - xsadd_float(xsadd);
    }

    /**
     * This function outputs double precision floating point number from
     * internal state.
     * @param[in,out] xsadd xsadd internal state
     * @return floating point number r (0.0 < r <= 1.0)
     */
    inline static double xsadd_double(xsadd_t * xsadd)
    {
	uint64_t a = xsadd_uint32(xsadd) & UINT32_C(0xffffffe0);
	uint64_t b = xsadd_uint32(xsadd);
	a = (a << 32) | (b << 5);
	return a * XSADD_DOUBLE_MUL;
    }

    /* =============
     * JUMP function
     * ============= */
    /**
     * Recomended large jump step.
     * The value is 3^41.
     */
    extern const char * const xsadd_jump_base_step;

    /**
     * jump function
     * @param[in, out] xsadd xsadd structure, overwritten by new state
     * after calling this function.
     * @param[in] mul_step jump step is mul_step * base_step.
     * @param[in] base_step hexadecimal number string less than 2<sup>128</sup>.
     */
    void xsadd_jump(xsadd_t * xsadd, uint32_t mul_step, const char * base_step);

    /**
     * jump using the jump polynomial.  This function is not as time
     * consuming as calculating jump polynomial.  This function can
     * use multiple times for the xsadd structure.
     * @param[in, out] xsadd xsadd structure, overwritten by new state
     * after calling this function.
     * @param[in] jump_poly the jump polynomial calculated by
     * xsadd_calculate_jump_polynomial.
     */
    void xsadd_jump_by_polynomial(xsadd_t * xsadd, const char * jump_str);

    /**
     * calculate jump polynomial.
     * This function is time consuming.
     * jump_str needs 33 bytes memory.
     *
     * @param[out] jump_str the result of this calculation.
     * @param[in] mul_step jump step is mul_step * base_step.
     * @param[in] base_step hexadecimal string of jump base.
     */
    void xsadd_calculate_jump_polynomial(char * jump_str,
					 uint32_t mul_step,
					 const char * base_step);


#ifdef __cplusplus
}
#endif

#undef XSADD_FLOAT_MUL
#undef XSADD_DOUBLE_MUL

#endif // XSADD_H
