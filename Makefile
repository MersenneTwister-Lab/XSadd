#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

CC = gcc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = icc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = clang -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)

all:  check32 check64

test:  test_xsadd.c xsadd.o
	${CC} -o $@  test_xsadd.c xsadd.o ${LINKOPT}


test_small:  test_xsadd_small.c xsadd_small.o
	${CC} -o $@  test_xsadd_small.c xsadd_small.o ${LINKOPT}

.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM html
