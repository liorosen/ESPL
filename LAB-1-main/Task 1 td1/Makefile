CFLAGS=-g -Wall -m32

all: td1

td1: td1.o
	gcc $(CFLAGS) -o td1 td1.o

td1.o: td1.c
	gcc $(CFLAGS) -c -o td1.o td1.c

clean:
	rm -f td1 td1.o