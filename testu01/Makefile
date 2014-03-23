#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

CC = gcc -I../ -Wall -Wextra -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = icc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)

crushes-xsadd: crushes-xsadd.c crushes.c ../xsadd.h ../xsadd.c
	${CC} -DMAIN -o $@ crushes-xsadd.c crushes.c ../xsadd.c -ltestu01


.c.o:
	${CC} -c $<

clean:
	rm -rf *.o *~ *.dSYM html
