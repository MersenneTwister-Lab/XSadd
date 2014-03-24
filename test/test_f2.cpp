#include <NTL/GF2X.h>
#include "xsadd.h"
#include "xsadd.c" // to check static functions
#include <stdint.h>
#include <sstream>
#include <UnitTest++.h>
#include <random>

using namespace NTL;
using namespace std;

char buff[200];

bool eq(f2_polynomial& a, f2_polynomial& b)
{
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	if (a.ar[i] != b.ar[i]) {
	    return false;
	}
    }
    return true;
}

void tof2(f2_polynomial& a, GF2X& b)
{
    clear(&a);
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	for (int j = 0; j < 32; j++) {
	    uint32_t mask = 1 << j;
	    if (IsOne(coeff(b, i * 32 + j))) {
		a.ar[i] |= mask;
	    }
	}
    }
}

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

void toGF2X(GF2X& result, f2_polynomial * x)
{
    clear(result);
    int index = 0;
    for (int i = 0; i < POLYNOMIAL_ARRAY_SIZE; i++) {
	uint32_t mask;
	mask = 1;
	for (int j = 0; j < 32; j++) {
	    if (mask & x->ar[i]) {
		SetCoeff(result, index, 1);
	    } else {
		SetCoeff(result, index, 0);
	    }
	    index++;
	    mask = mask << 1;
	}
    }
}

void set(ZZ& step, const char * step_str)
{
    stringstream ss;
    ss << step_str;
    ss >> step;
}

void set(GF2X& ch, const char * characteristic_polynomial)
{
    f2_polynomial x;
    strtopolynomial(&x, characteristic_polynomial);
    toGF2X(ch, &x);
}

SUITE(F2POLYNOMIAL) {
    TEST(ADD)
    {
	GF2X p;
	GF2X q;
	GF2X r;
	f2_polynomial p2;
	f2_polynomial q2;
	f2_polynomial r2;
	for (int i = 0; i < 100; i++) {
	    random(p, POLYNOMIAL_ARRAY_SIZE * 32 - 1);
	    random(q, POLYNOMIAL_ARRAY_SIZE * 32 - 1);
	    r = p + q;
	    tof2(p2, p);
	    tof2(q2, q);
	    tof2(r2, r);
	    add(&p2, &q2);
	    CHECK(eq(p2, r2));
	}
    }
    TEST(MUL)
    {
	GF2X p;
	GF2X q;
	GF2X r;
	f2_polynomial p2;
	f2_polynomial q2;
	f2_polynomial r2;
	for (int i = 0; i < 100; i++) {
	    random(p, POLYNOMIAL_ARRAY_SIZE * 16 - 1);
	    random(q, POLYNOMIAL_ARRAY_SIZE * 16 - 1);
	    r = p * q;
	    tof2(p2, p);
	    tof2(q2, q);
	    tof2(r2, r);
	    mul(&p2, &q2);
	    CHECK(eq(p2, r2));
	    if (!eq(p2, r2)) {
		printf("deg p = %ld\n", deg(p));
		printf("deg q = %ld\n", deg(q));
		printf("deg r = %ld\n", deg(r));
		break;
	    }
	}
    }
    TEST(MOD)
    {
	GF2X p;
	GF2X q;
	GF2X r;
	f2_polynomial p2;
	f2_polynomial q2;
	f2_polynomial r2;
	for (int i = 0; i < 100; i++) {
	    random(p, POLYNOMIAL_ARRAY_SIZE * 32 - 1);
	    random(q, POLYNOMIAL_ARRAY_SIZE * 32 - 1);
	    r = p % q;
	    tof2(p2, p);
	    tof2(q2, q);
	    tof2(r2, r);
	    mod(&p2, &q2);
	    CHECK(eq(p2, r2));
	    if (!eq(p2, r2)) {
		printf("deg p = %ld\n", deg(p));
		printf("deg q = %ld\n", deg(q));
		printf("deg r = %ld\n", deg(r));
		break;
	    }
	}
    }
    TEST(SQUARE)
    {
	GF2X p;
	GF2X r;
	f2_polynomial p2;
	f2_polynomial r2;
	for (int i = 0; i < 100; i++) {
	    random(p, POLYNOMIAL_ARRAY_SIZE * 16 - 1);
	    r = p * p;
	    tof2(p2, p);
	    tof2(r2, r);
	    square(&p2);
	    CHECK(eq(p2, r2));
	    if (!eq(p2, r2)) {
		printf("deg p = %ld\n", deg(p));
		printf("deg r = %ld\n", deg(r));
		break;
	    }
	}
    }
    TEST(POWERMOD)
    {
	GF2X p;
	GF2X q;
	ZZ x;
	GF2X r;
	f2_polynomial p2;
	f2_polynomial q2;
	uz x2;
	f2_polynomial r2;
	f2_polynomial r3;
	for (int i = 0; i < 100; i++) {
	    random(p, POLYNOMIAL_ARRAY_SIZE * 16 - 1);
	    random(q, POLYNOMIAL_ARRAY_SIZE * 16 - 1);
	    p = p % q;
	    RandomBits(x, 10);
	    r = PowerMod(p, x, q);
	    tof2(p2, p);
	    tof2(q2, q);
	    tof2(r2, r);
	    touz(&x2, x);
	    polynomial_power_mod(&r3, &p2, &x2, &q2);
	    CHECK(eq(r2, r3));
	    if (!eq(r2, r3)) {
		printf("deg p = %ld\n", deg(p));
		printf("deg q = %ld\n", deg(q));
		printf("deg r = %ld\n", deg(r));
		fflush(stdout);
		cout << "x:" << hex << x << endl;
		uztostring16(buff, &x2);
		printf("x2:%s\n", buff);
		break;
	    }
	}
    }
    TEST(POWERMOD2)
    {
	ZZ step;
	const char * step_str = "48594547533363904080606289765";
	//const char * step_str = "48594547533363904080606289766";
	set(step, step_str);
	GF2X ch;
	set(ch, characteristic_polynomial);
	GF2X q;
	PowerXMod(q, step, ch);
	f2_polynomial p2;
	f2_polynomial q2;
	uz step2;
	clear(&p2);
	p2.ar[0] = 2;
	touz(&step2, step);
	f2_polynomial ch2;
	strtopolynomial(&ch2, characteristic_polynomial);
	f2_polynomial result;
	polynomial_power_mod(&q2, &p2, &step2, &ch2);
	tof2(result, q);
	CHECK(eq(result, q2));
	if (!eq(result, q2)) {
	    cout << "powermod2 error" << endl;
	    polynomialtostr(buff, &result);
	    cout << "result:" << buff << endl;
	    polynomialtostr(buff, &q2);
	    cout << "q2:    " << buff << endl;
	}
    }
}

void str16(char * buff, const ZZ& a)
{
    if (IsZero(a)) {
	strcpy(buff, "0");
	return;
    }
    const char tbl[] = {'0','1','2','3','4','5','6','7','8','9',
			'a','b','c','d','e','f'};
    stringstream ss;
    ZZ b = a;
    ZZ c;
    while(!IsZero(b)) {
	c = b % 16;
	long t = trunc_long(c, 100);
	ss << tbl[t % 16];
	b = b / 16;
    }
    string str;
    ss >> str;
    string result(str.rbegin(), str.rend());
    strncpy(buff, result.c_str(), 200);
}

bool eq(uz& a, uz& b)
{
    for (int i = 0; i < UZ_ARRAY_SIZE; i++) {
	if (a.ar[i] != b.ar[i]) {
	    return false;
	}
    }
    return true;
}

void touz(uz& a, const ZZ& x)
{
    ZZ work = x;
    for (int i = 0; i < UZ_ARRAY_SIZE; i++) {
	a.ar[i] = 0;
    }
    for (int i = 0; i < UZ_ARRAY_SIZE; i++) {
	for (int j = 0; j < 16; j++) {
	    uint16_t mask = 1 << j;
	    if (IsOdd(work)) {
		a.ar[i] |= mask;
	    }
	    work = work >> 1;
	}
    }
}


SUITE(UZ) {
    TEST(STRTOUZ0)
    {
	ZZ x;
	uz x2;
	uz r;
	x = 0;
	str16(buff, x);
	touz(x2, x);
	string16touz(&r, buff);
	CHECK(eq(x2, r));
	if (!eq(x2, r)) {
	    cout << "s: " << buff << endl;
	}
    }
    TEST(STRTOUZ)
    {
	ZZ x;
	uz x2;
	uz r;
	for (int i = 0; i < 100; i++) {
	    RandomBits(x, UZ_ARRAY_SIZE * 16 - 1);
	    str16(buff, x);
	    touz(x2, x);
	    string16touz(&r, buff);
	    CHECK(eq(x2, r));
	    if (!eq(x2, r)) {
		cout << "s: " << buff << endl;
		break;
	    }
	}
    }
    TEST(UZTOSTR)
    {
	ZZ x;
	uz x2;
	uz r;
	for (int i = 0; i < 100; i++) {
	    RandomBits(x, UZ_ARRAY_SIZE * 16 - 1);
	    touz(x2, x);
	    uztostring16(buff, &x2);
	    string16touz(&r, buff);
	    CHECK(eq(x2, r));
	    if (!eq(x2, r)) {
		cout << "s: " << buff << endl;
		break;
	    }
	}
    }
    TEST(MUL0)
    {
	ZZ x;
	ZZ y;
	ZZ r;
	uz x2;
	uz y2;
	uz r2;
	uz r3;
	for (int i = 0; i < 100; i++) {
	    RandomBits(x, UZ_ARRAY_SIZE * 16 - 1);
	    y = 0;
	    //RandomBits(y, UZ_ARRAY_SIZE * 16 - 1);
	    r = x * y;
	    touz(x2, x);
	    touz(y2, y);
	    touz(r2, r);
	    uz_mul(&r3, &x2, &y2);
	    CHECK(eq(r3, r2));
	    if (!eq(r3, r2)) {
		cout << "x: " << hex << x;
		cout << "y: " << hex << y;
		cout << "r: " << hex << r;
		break;
	    }
	}
    }
    TEST(MUL)
    {
	ZZ x;
	ZZ y;
	ZZ r;
	uz x2;
	uz y2;
	uz r2;
	uz r3;
	for (int i = 0; i < 100; i++) {
	    RandomBits(x, UZ_ARRAY_SIZE * 16 - 1);
	    RandomBits(y, UZ_ARRAY_SIZE * 16 - 1);
	    r = x * y;
	    touz(x2, x);
	    touz(y2, y);
	    touz(r2, r);
	    uz_mul(&r3, &x2, &y2);
	    CHECK(eq(r3, r2));
	    if (!eq(r3, r2)) {
		cout << "x: " << hex << x;
		cout << "y: " << hex << y;
		cout << "r: " << hex << r;
		break;
	    }
	}
    }
}


void tostr(char * str, GF2X& poly)
{
    f2_polynomial x;
    tof2(x, poly);
    polynomialtostr(str, &x);
}

SUITE(JUMPPOL)
{
    TEST(CALCULATE)
    {
	/* xsadd_calculate_jump_polynomial */
	mt19937 mt(1);
	ZZ step;
	ZZ step3;
	uint32_t mul_step;
	uz step2;
	char base_string[200];
	char jump_string[200];
	char jump_string2[200];
	GF2X xpoly;
	GF2X xchar;
	f2_polynomial characteristic;
	strtopolynomial(&characteristic, characteristic_polynomial);
	toGF2X(xchar, &characteristic);
	for (int i = 0; i < 100; i++) {
	    //cout << "test calculate no." << dec << i << endl;
	    mul_step = mt();
	    RandomBits(step, 65);
	    touz(step2, step);
	    uztostring16(base_string, &step2);
	    xsadd_calculate_jump_polynomial(jump_string,
					    mul_step,
					    base_string);
	    step3 = step * mul_step;
	    //cout << "step: " << step << endl;
	    PowerXMod(xpoly, step3, xchar);
	    //cout << "xpoly: " << xpoly << endl;
	    //cout << "deg(xpoly): " << dec << deg(xpoly) << endl;
	    tostr(jump_string2, xpoly);
	    //cout << "jump_string:" << jump_string << endl;
	    //cout << "jump_string2:" << jump_string2 << endl;
	    CHECK(strncmp(jump_string2, jump_string, 200) == 0);
	    //cout << "mul_step :" << dec << mul_step << endl;
	    //cout << "base_string:" << base_string << endl;
	    if (strcmp(jump_string2, jump_string) != 0) {
		cout << "jump_string: " << jump_string << endl;
		cout << "jump_string2:" << jump_string2 << endl;
		cout << "i = " << dec << i << endl;
		cout << "mul_step :" << dec << mul_step << endl;
		cout << "base_string:" << base_string << endl;
		cout << "step: " << step << endl;
		uztostring16(buff, &step2);
		cout << "step2:" << buff << endl;
		cout << "step3 = jump_step:" << step3 << endl;
		uz tmp;
		uint32touz(&tmp, mul_step);
		uz tmp2;
		uz_mul(&tmp2, &step2, &tmp);
		uztostring16(buff, &tmp2);
		cout << "step3uz:" << buff << endl;
		break;
	    }
	}
    }
}
