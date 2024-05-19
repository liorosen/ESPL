CFLAGS=-g -Wall -m32

all: base

base: base.o
	gcc $(CFLAGS) -o base base.o

base.o: base.c
	gcc $(CFLAGS) -c -o base.o base.c

clean:
	rm -f base base.o
