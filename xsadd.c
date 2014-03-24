/**
 * @file xsadd.c
 *
 * @brief XORSHIFT-ADD: 128-bit internal state pseudorandom number generator.
 *
 * @author Mutsuo Saito (Manieth Corp.)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (c) 2014
 * Mutsuo Saito, Makoto Matsumoto, Manieth Corp.,
 * and Hiroshima University.
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
#include <xsadd.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>

#define LOOP 8
#define POLYNOMIAL_ARRAY_SIZE 8
#define UZ_ARRAY_SIZE 8

/*
 * this is hexadecimal string
 */
static const char * const characteristic_polynomial
= "100000000008101840085118000000001";

/* 3^41 > 2^64 and 3^41 < 2^65 */
const char * const xsadd_jump_base_step = "1FA2A1CF67B5FB863";

/**
 * Polynomial over F<sub>2</sub>
 * LSB of ar[0], i.e. ar[0] & 1, represent constant
 */
struct F2_POLYNOMIAL_T {
    uint32_t ar[POLYNOMIAL_ARRAY_SIZE];
};

typedef struct F2_POLYNOMIAL_T f2_polynomial;
/**
 * unsigned integer
 * LSB of ar[0], i.e. ar[0] & 1, represent constant
 */
struct UZ_T {
    uint16_t ar[UZ_ARRAY_SIZE];
};

typedef struct UZ_T uz;

static void period_certification(xsadd_t * xsadd);
static void xsadd_add(xsadd_t *dest, const xsadd_t *src);


static void string16touz(uz * result, const char * str);
//static void uztostring16(char * str, uz * x);
static void uz_mul(uz * result, const uz * x, const uz * y);
static void uint32touz(uz * x, const uint32_t y);
static void uz_clear(uz * result);

static void strtopolynomial(f2_polynomial * poly, const char * str);
static void polynomialtostr(char * str, f2_polynomial * poly);
static void polynomial_power_mod(f2_polynomial * dest,
				 const f2_polynomial * x,
				 uz * power,
				 const f2_polynomial * mod);
static void mod(f2_polynomial *dest, const f2_polynomial *x);
inline static void shiftup1(f2_polynomial *dest);
inline static int deg(const f2_polynomial * x);
inline static int deg_lazy(const f2_polynomial * x, const int pre_deg);
inline static void clear(f2_polynomial * dest);
inline static uint32_t ini_func1(uint32_t x);
inline static uint32_t ini_func2(uint32_t x);

/* ================
 * PUBLIC FUNCTIONS
   ================ */
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
        xsadd_next_state(xsadd);
    }
}

void xsadd_init_by_array(xsadd_t * random,
			 const uint32_t init_key[],
			 int key_length)
{
    const int lag = 1;
    const int mid = 1;
    const int size = 4;
    int i, j;
    int count;
    uint32_t r;
    uint32_t * st = &random->state[0];

    st[0] = 0;
    st[1] = 0;
    st[2] = 0;
    st[3] = 0;
    if (key_length + 1 > LOOP) {
	count = key_length + 1;
    } else {
	count = LOOP;
    }
    r = ini_func1(st[0] ^ st[mid % size]
		  ^ st[(size - 1) % size]);
    st[mid % size] += r;
    r += key_length;
    st[(mid + lag) % size] += r;
    st[0] = r;
    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = ini_func1(st[i % size]
		      ^ st[(i + mid) % size]
		      ^ st[(i + size - 1) % size]);
	st[(i + mid) % size] += r;
	r += init_key[j] + i;
	st[(i + mid + lag) % size] += r;
	st[i % size] = r;
	i = (i + 1) % size;
    }
    for (; j < count; j++) {
	r = ini_func1(st[i % size]
		      ^ st[(i + mid) % size]
		      ^ st[(i + size - 1) % size]);
	st[(i + mid) % size] += r;
	r += i;
	st[(i + mid + lag) % size] += r;
	st[i % size] = r;
	i = (i + 1) % size;
    }
    for (j = 0; j < size; j++) {
	r = ini_func2(st[i % size]
		      + st[(i + mid) % size]
		      + st[(i + size - 1) % size]);
	st[(i + mid) % size] ^= r;
	r -= i;
	st[(i + mid + lag) % size] ^= r;
	st[i % size] = r;
	i = (i + 1) % size;
    }
    period_certification(random);
    for (i = 0; i < LOOP; i++) {
	xsadd_next_state(random);
    }
}

/**
 * jump function
 * @param xsadd xsadd structure, overwritten by new state after calling
 * this function.
 * @param mul_step jump step is mul_step * base_step.
 * @param base_step hexadecimal number string less than 2<sup>128</sup>.
 * xsadddc
 */
void xsadd_jump(xsadd_t *xsadd,
		uint32_t mul_step,
		const char * base_step)
{
    char jump_str[200];
    xsadd_calculate_jump_polynomial(jump_str, mul_step, base_step);
    xsadd_jump_by_polynomial(xsadd, jump_str);
}

/**
 * jump using the jump polynomial.
 * This function is not as time consuming as calculating jump polynomial.
 * This function can use multiple time for the xsadd structure.
 * @param xsadd xsadd structure, overwritten by new state after calling
 * this function.
 * @param jump_str the jump polynomial calculated by
 * xsadd_calculate_jump_polynomial.
 */
void xsadd_jump_by_polynomial(xsadd_t *xsadd, const char * jump_str)
{
    f2_polynomial jump_poly;
    xsadd_t work_z;
    xsadd_t * work = &work_z;
    *work = *xsadd;
    for (int i = 0; i < 4; i++) {
        work->state[i] = 0;
    }
    strtopolynomial(&jump_poly, jump_str);
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	for (int j = 0; j < 32; j++) {
	    uint32_t mask = 1 << j;
	    if ((jump_poly.ar[i] & mask) != 0) {
		xsadd_add(work, xsadd);
	    }
	    xsadd_next_state(xsadd);
	}
    }
    *xsadd = *work;
}

void xsadd_calculate_jump_polynomial(char *jump_str,
				     uint32_t mul_step,
				     const char * base_step)
{
    f2_polynomial jump_poly;
    f2_polynomial charcteristic;
    f2_polynomial tee;
    uz base;
    uz mul;
    uz step;

    strtopolynomial(&charcteristic, characteristic_polynomial);
    clear(&tee);
    tee.ar[0] = 2;
    string16touz(&base, base_step);
    uint32touz(&mul, mul_step);
    uz_mul(&step, &mul, &base);
    polynomial_power_mod(&jump_poly, &tee, &step, &charcteristic);
    polynomialtostr(jump_str, &jump_poly);
}

/* ================
 * PRIVATE FUNCTIONS
   ================ */
/**
 * Addition of internal state as F<sub>2</sub> vector.
 * @param dest destination
 * @param src source
 */
static void xsadd_add(xsadd_t *dest, const xsadd_t *src)
{
    dest->state[0] ^= src->state[0];
    dest->state[1] ^= src->state[1];
    dest->state[2] ^= src->state[2];
    dest->state[3] ^= src->state[3];
}

static void period_certification(xsadd_t * xsadd)
{
    if (xsadd->state[0] == 0 &&
        xsadd->state[1] == 0 &&
        xsadd->state[2] == 0 &&
        xsadd->state[3] == 0) {
        xsadd->state[0] = 'X';
        xsadd->state[1] = 'S';
        xsadd->state[2] = 'A';
        xsadd->state[3] = 'D';
    }
}


/**
 * addition of F<sub>2</sub>-polynomial<br>
 * dest = dest + src
 * @param dest destination polynomial
 * @param src source polynomial
 */
inline static void add(f2_polynomial * dest, const f2_polynomial * src)
{
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	dest->ar[i] ^= src->ar[i];
    }
}

/**
 * clear polynomial.
 * dest = 0
 * @param dest polynomial set to be zero.
 */
inline static void clear(f2_polynomial * dest)
{
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	dest->ar[i] = 0;
    }
}

/**
 * shift up 1 bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b>
 * @param dest 256-bit polynomial
 */
inline static void shiftup1(f2_polynomial *dest)
{
    uint32_t lsb = 0;
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	uint32_t tmp = dest->ar[i] >> 31;
	dest->ar[i] = (dest->ar[i] << 1) | lsb;
	lsb = tmp;
    }
}

/**
 * shift up n bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t<sup>n</sup></b>
 * @param dest 256-bit polynomial
 * @param n shift value
 */
inline static void shiftup0n(f2_polynomial *dest, int n)
{
    uint32_t lsb = 0;
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	uint32_t tmp = dest->ar[i] >> (32 - n);
	dest->ar[i] = (dest->ar[i] << n) | lsb;
	lsb = tmp;
    }
}

/**
 * shift down 1 bit, if　indeterminate of dest is <b>t</b>
 * dest = dest / <b>t</b> and constant is truncated.
 * @param dest 256-bit polynomial
 */
inline static void shiftdown1(f2_polynomial *dest)
{
    uint32_t msb = 0;
    for (int i = POLYNOMIAL_ARRAY_SIZE - 1; i >= 0; i--) {
	uint32_t tmp = dest->ar[i] << 31;
	dest->ar[i] = (dest->ar[i] >> 1) | msb;
	msb = tmp;
    }
}

/**
 * shift up n bit, if　indeterminate of dest is <b>t</b>
 * dest = dest * <b>t</b><sup>n</sup>
 * @param dest 256-bit polynomial
 * @param n number of shift up
 */
inline static void shiftup(f2_polynomial * dest, int n)
{
    int q = n / 32;
    int r = n % 32;
    if (q == 0) {
	shiftup0n(dest, r);
	return;
    }
    if (q >= POLYNOMIAL_ARRAY_SIZE) {
	clear(dest);
	return;
    }
    for (int i = POLYNOMIAL_ARRAY_SIZE - 1; i >= q; i--) {
	uint32_t lower;
	if ((r != 0) && (i - q - 1 >= 0)) {
	    lower = dest->ar[i - q - 1] >> (32 - r);
	} else {
	    lower = 0;
	}
	dest->ar[i] = dest->ar[i - q] << r | lower;
    }
    for (int i = q - 1; i >= 0; i--) {
	dest->ar[i] = 0;
    }
}

/**
 * get degree
 * if polynomial is zero return -1, else return degree of the polynomial.
 * @param x polynomial
 * @return degree
 */
inline static int deg(const f2_polynomial * x)
{
    return deg_lazy(x, POLYNOMIAL_ARRAY_SIZE * 32 - 1);
}

/**
 * sub function
 * pre_deg >= deg (must be)
 * @param x polynomial
 * @param pre_deg search start degree
 * @return degree
 */
inline static int deg_lazy(const f2_polynomial * x, const int pre_deg)
{
    int deg = pre_deg;
    uint32_t mask;
    int index = pre_deg / 32;
    int bit_pos = pre_deg % 32;
    if (index >= POLYNOMIAL_ARRAY_SIZE) {
	index = POLYNOMIAL_ARRAY_SIZE - 1;
	bit_pos = 31;
	deg = (index + 1) * 32 - 1;
    }
    mask = UINT32_C(1) << bit_pos;
    for (int i = index; i >= 0; i--) {
	while (mask != 0) {
	    if ((x->ar[i] & mask) != 0) {
		return deg;
	    }
	    mask = mask >> 1;
	    deg--;
	}
	mask = UINT64_C(1) << 31;
    }
    return -1;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
inline static uint32_t ini_func1(uint32_t x)
{
    return (x ^ (x >> 27)) * UINT32_C(1664525);
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
inline static uint32_t ini_func2(uint32_t x)
{
    return (x ^ (x >> 27)) * UINT32_C(1566083941);
}

/**
 * multiplication of polynomials
 * y's degree is assumed to be lower than 128 <br>
 * x = x * y
 * @param x polynomial
 * @param y polynomial
 */
static void mul(f2_polynomial *x, const f2_polynomial *y)
{
    f2_polynomial result_z;
    f2_polynomial *result = &result_z;
    clear(result);
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	uint32_t u = y->ar[i];
	for (int j = 0; j < 32; j++) {
	    if ((u & 1) != 0) {
		add(result, x);
	    }
	    shiftup1(x);
	    u = u >> 1;
	}
    }
    *x = *result;
}

/**
 * square polynomial <br>
 * x = x * x
 * @param x polynomial
 */
static void square(f2_polynomial *x)
{
    f2_polynomial tmp;
    tmp = *x;
    mul(x, &tmp);
}

/**
 * remainder of polynomial
 * dest = dest % x
 * @param dest polynomial to be divided and remainder
 * @param x divisor
 */
static void mod(f2_polynomial *dest, const f2_polynomial *x)
{
    f2_polynomial tmp_z;
    f2_polynomial * tmp = &tmp_z;
    int degree = deg(x);
    int dest_deg = deg(dest);
    int diff = dest_deg - degree;
    int tmp_deg = degree;
    if (diff < 0) {
	return;
    }
    *tmp = *x;
    if (diff == 0) {
	add(dest, tmp);
	return;
    }
    shiftup(tmp, diff);
    tmp_deg += diff;
    add(dest, tmp);
    dest_deg = deg_lazy(dest, dest_deg);
    while (dest_deg >= degree) {
	shiftdown1(tmp);
	tmp_deg--;
	if (dest_deg == tmp_deg) {
	    add(dest, tmp);
	    dest_deg = deg_lazy(dest, dest_deg);
	}
    }
}

/**
 * conversion from string to polynomial.
 * string must be in the format of TinyMTDC outputs.
 * @param poly 128-bit polynomial
 * @param str hexadecimal string of 128-bit polynomial.
 */
static void strtopolynomial(f2_polynomial * poly, const char * str)
{
    char buffer[POLYNOMIAL_ARRAY_SIZE * 8 + 1];
    strncpy(buffer, str, POLYNOMIAL_ARRAY_SIZE * 8);
    buffer[POLYNOMIAL_ARRAY_SIZE * 8] = 0;
    errno = 0;
    int len = strlen(buffer);
    int pos = len - 8;
    int i;
    for (i = 0; pos >= 0 && i < POLYNOMIAL_ARRAY_SIZE; i++) {
	poly->ar[i] = strtoul(buffer + pos, NULL, 16);
	if (errno) {
	    /* throw exception */
	    perror(__FILE__);
	}
	buffer[pos] = 0;
	pos -= 8;
    }
    if (pos + 8 > 0) {
	poly->ar[i] = strtoul(buffer, NULL, 16);
	i++;
	if (errno) {
	    /* throw exception */
	    perror(__FILE__);
	}
    }
    for (; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	poly->ar[i] = 0;
    }
}

/**
 * conversion from polynomial to string.
 * string must be in the format of TinyMTDC outputs.
 * @param poly 128-bit polynomial
 * @param str hexadecimal string of 128-bit polynomial.
 */
static void polynomialtostr(char * str, f2_polynomial * poly)
{
    int first = 1;
    int pos = 0;
    for (int i = POLYNOMIAL_ARRAY_SIZE - 1; i >= 0; i--) {
	if (first) {
	    if (poly->ar[i] != 0) {
		sprintf(str + pos, "%x", poly->ar[i]);
		pos = strlen(str);
		first = 0;
	    }
	} else {
	    sprintf(str + pos, "%08x", poly->ar[i]);
	    pos = strlen(str);
	}
    }
    if (first) {
	strcpy(str, "0");
    }
}

/**
 * dest = x<sup>power</sup> % mod
 * @param dest the result of calculation
 * @param x polynomial
 * @param power exponential part
 * @param mod_poly divisor polynomial
 */
static void polynomial_power_mod(f2_polynomial * dest,
				 const f2_polynomial * x,
				 uz * power,
				 const f2_polynomial * mod_poly)
{
    f2_polynomial tmp_z;
    f2_polynomial * tmp = &tmp_z;
    f2_polynomial result_z;
    f2_polynomial * result = & result_z;
    *tmp = *x;
    clear(result);
    result_z.ar[0] = 1;
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	uint16_t tmp_power = power->ar[i];
	for (int j = 0; j < 16; j++) {
	    if ((tmp_power & 1) != 0) {
		mul(result, tmp);
		mod(result, mod_poly);
	    }
	    square(tmp);
	    mod(tmp, mod_poly);
	    tmp_power = tmp_power >> 1;
	}
    }
    *dest = *result;
}

static void uz_clear(uz *a)
{
   for (int i = 0; i < UZ_ARRAY_SIZE; i++) {
	a->ar[i] = 0;
    }
}

static void uint32touz(uz * x, const uint32_t y)
{
    uz_clear(x);
    x->ar[0] = y & 0xffff;
    x->ar[1] = y >> 16;
}

static void uz_mul(uz * result, const uz * x, const uz * y)
{
    uint32_t tmp;
    const uint32_t lmask = 0xffffU;
    uz_clear(result);
    for (int i = 0; i < UZ_ARRAY_SIZE; i++) {
	for (int j = 0; j < UZ_ARRAY_SIZE; j++) {
	    tmp = x->ar[i] * y->ar[j];
	    if (i + j >= UZ_ARRAY_SIZE) {
		break;
	    }
	    tmp += result->ar[i+j];
	    result->ar[i+j] = (uint16_t)(tmp & lmask);
	    for (int k = i + j + 1; k < UZ_ARRAY_SIZE; k++) {
		tmp = tmp >> 16;
		if (tmp == 0) {
		    break;
		}
		tmp += result->ar[k];
		result->ar[k] = (uint16_t)(tmp & lmask);
	    }
	}
    }
}

static void string16touz(uz * result, const char * str)
{
    errno = 0;
    char s[4 * UZ_ARRAY_SIZE + 1];
    strncpy(s, str, 4 * UZ_ARRAY_SIZE);
    s[4 * UZ_ARRAY_SIZE] = '\0';
    int len = strlen(s);
    uz_clear(result);
    for (int i = 0; i < UZ_ARRAY_SIZE; i++) {
	len = len - 4;
	if (len < 0) {
	    len = 0;
	}
	uint16_t tmp = (uint16_t)strtoul(s + len, NULL, 16);
	if (errno) {
	    perror("string16touz");
	    exit(1);
	}
	result->ar[i] = tmp;
	if (len == 0) {
	    break;
	}
	s[len] = '\0';
    }
}

#if 0
static void uztostring16(char * str, uz * x)
{
    int p = 0;
    int first = 1;
    for (int i = UZ_ARRAY_SIZE - 1; i >= 0; i--) {
	if (first) {
	    if (x->ar[i] != 0) {
		first = 0;
		sprintf(str, "%x", x->ar[i]);
		p = strlen(str);
	    }
	} else {
	    sprintf(str + p, "%04x", x->ar[i]);
	    p = p + 4;
	}
    }
    if (first) {
	strcpy(str, "0");
    }
}
#endif
