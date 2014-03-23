#include <stdio.h>
#include "crushes.h"
#include "xsadd.h"

xsadd_t xsadd;

uint32_t test_generator()
{
    return xsadd_uint32(&xsadd);
}

void test_init(uint32_t seed)
{
    xsadd_init(&xsadd, seed);
#if 0
    printf("xsadd seed = %d\n", seed);
    for (int i = 0; i < 4; i++) {
	for (int j = 0; j < 4; j++) {
	    printf("%10u ", xsadd_uint32(&xsadd));
	}
    }
    printf("xsadd_init(&xsadd, %d)\n", seed);
    xsadd_init(&xsadd, seed);
#endif
}

char * test_name()
{
    return "xsadd";
}
