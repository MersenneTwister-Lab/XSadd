#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1
CCOPTION = -I. -Wall -Wextra -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
CC = gcc
#CC = icc
#CC = clang

all: test_xsadd doc

test_xsadd:  test_xsadd.c xsadd.o
	${CC} ${CCOPTION} -o $@  test_xsadd.c xsadd.o

doc:xsadd.h doxygen.cfg
	doxygen doxygen.cfg

xsadd.c: xsadd.h

.c.o:
	${CC} ${CCOPTION} -c $<

clean:
	rm -rf *.o *~ *.dSYM html test_xsadd
