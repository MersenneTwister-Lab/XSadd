#include <NTL/GF2X.h>
#include <stdint.h>
#include <UnitTest++.h>
#include <tr1/random>
#include <sstream>
#include "xsadd.h"
#include "xsadd.c" // to check static functions

using namespace NTL;
using namespace std;

void touz(uz *r, ZZ& x)
{
    ZZ work = x;
    uz_clear(r);
    for (int i = 0; i < UZ_ARRAY_SIZE; i++) {
	for (int j = 0; j < 16; j++) {
	    uint16_t mask = 1 << j;
	    if (IsOdd(work)) {
		r->ar[i] |= mask;
	    }
	    work = work >> 1;
	}
    }
}

void uztostring16(char * str, uz * x)
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

void to16string(char * buff, ZZ& x)
{
    uz tmp;
    touz(&tmp, x);
    uztostring16(buff, &tmp);
}

bool eq(xsadd_t& xs1, xsadd_t& xs2)
{
    for (int i = 0; i < 4; i++) {
	if (xs1.state[i] != xs2.state[i]) {
	    return false;
	}
    }
    return true;
}

SUITE(JUMP) {
    TEST(SMALL)
    {
	xsadd_t xs1;
	xsadd_t xs2;
	tr1::mt19937 mt(1);
	for (int i = 0; i < 200; i++) {
	    uint32_t seed = mt();
	    int step = mt() % 500;
	    xsadd_init(&xs1, seed);
	    xsadd_init(&xs2, seed);
	    xsadd_jump(&xs1, step, "1");
	    for (int j = 0; j < step; j++) {
		xsadd_uint32(&xs2);
	    }
	    CHECK(eq(xs1, xs2));
	}
    }
    TEST(LARGE)
    {
	xsadd_t xs1;
	xsadd_t xs2;
	tr1::mt19937 mt(1);
	char buff[200];
	for (int i = 0; i < 200; i++) {
	    uint32_t seed = mt();
	    int step = mt() % 500;
	    ZZ lstep;
	    RandomBits(lstep, 70);
	    to16string(buff, lstep);
	    xsadd_init(&xs1, seed);
	    xsadd_init(&xs2, seed);
	    xsadd_jump(&xs1, step, buff);
	    lstep *= step;
	    to16string(buff, lstep);
	    xsadd_jump(&xs2, 1, buff);
	    CHECK(eq(xs1, xs2));
	}
    }
}
