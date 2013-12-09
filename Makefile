#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

CC = gcc -Wall -Wextra -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = icc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = clang -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)

all:  test_indexed test

test_indexed:  test_xsadd_indexed.c xsadd_indexed.o
	${CC} -o $@  test_xsadd_indexed.c xsadd_indexed.o

test_macro:  test_xsadd_indexed.c xsadd_indexed.o
	${CC} -DUSE_MACRO_SHIFT -o $@ test_xsadd_indexed.c xsadd_indexed.o

test_abcd:  test_xsadd_abcd.c xsadd_abcd.o
	${CC} -o $@  test_xsadd_abcd.c xsadd_abcd.o

test:  test_xsadd.c xsadd.o
	${CC} -o $@  test_xsadd.c xsadd.o


xsadd.c: xsadd.h
xsadd_indexd.c: xsadd_indexed.h

.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM html test test_indexed test_macro test_abcd
