#ifndef CRUSHES_H
#define CRUSHES_H
#include <stdint.h>

void crushes(char type, uint32_t seed);
uint32_t test_generator(void);
void test_init(uint32_t seed);
char * test_name(void);
#endif
