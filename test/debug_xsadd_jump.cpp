#include <iostream>
#include <iomanip>
#include <NTL/GF2X.h>
#include <stdint.h>
#include "xsadd.h"
#include "xsadd.c" // to check static functions

using namespace NTL;
using namespace std;

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

//static const char * const characteristic_polynomial
//= "100000000008101840085118000000001";

void xsadd_calculate_jump_polynomial_debug(char *jump_str,
					   uint32_t mul_step,
					   const char * base_step)
{
    f2_polynomial jump_poly;
    f2_polynomial charcteristic;
    f2_polynomial tee;
    uz base;
    uz mul;
    uz step;
    char buff[200];

    strtopolynomial(&charcteristic, characteristic_polynomial);
    clear(&tee);
    tee.ar[0] = 2;
    string16touz(&base, base_step);
    uint32touz(&mul, mul_step);

    uztostring16(buff, &mul);
    cout << "mul:" << buff << endl;
    uztostring16(buff, &base);
    cout << "base:" << buff << endl;
    uz_mul(&step, &mul, &base);
    uztostring16(buff, &step);
    cout << "step:" << buff << endl;
    polynomial_power_mod(&jump_poly, &tee, &step, &charcteristic);
    polynomialtostr(jump_str, &jump_poly);
}

void xsadd_jump_by_polynomial_debug(xsadd_t *xsadd, const char * jump_str)
{
    f2_polynomial jump_poly;
    xsadd_t work_z;
    xsadd_t * work = &work_z;
    cout << "xsadd_jump_by_polynomial_debug: step1" << endl;
    *work = *xsadd;
    for (int i = 0; i < 4; i++) {
        work->state[i] = 0;
    }
    strtopolynomial(&jump_poly, jump_str);
    cout << "xsadd_jump_by_polynomial_debug: step2" << endl;
    char buff[200];
    polynomialtostr(buff, &jump_poly);
    cout << "jump_poly:" << buff << endl;
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	for (int j = 0; j < 32; j++) {
	    //cout << "(i,j) = (" << dec << i << "," << j << ")" << endl;
	    uint32_t mask = 1 << j;
	    if ((jump_poly.ar[i] & mask) != 0) {
		xsadd_add(work, xsadd);
	    }
	    xsadd_uint32(xsadd);
	}
    }
    cout << "xsadd_jump_by_polynomial_debug: step3" << endl;
    *xsadd = *work;
}

void xsadd_jump_debug(xsadd_t *xsadd,
		      uint32_t mul_step,
		      const char * base_step)
{
    char jump_str[200];
    cout << "step1" << endl;
    xsadd_calculate_jump_polynomial_debug(jump_str, mul_step, base_step);
    cout << "jump_str:" << jump_str << endl;
    cout << "step2" << endl;
    xsadd_jump_by_polynomial_debug(xsadd, jump_str);
    cout << "step3" << endl;
}

void test_xsadd_jump_debug()
{
    const char * base_step = "0";
    uint32_t step = 139;
    uint32_t seed = 1791095845;
    xsadd_t xsadd;
    xsadd_init(&xsadd, seed);
    xsadd_jump_debug(&xsadd, step, base_step);
    cout << "debug end" << endl;
}

int main() {
    test_xsadd_jump_debug();
}
