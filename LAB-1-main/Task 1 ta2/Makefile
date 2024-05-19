CFLAGS=-g -Wall -m32

all: ta2

ta2: ta2.o
	gcc $(CFLAGS) -o ta2 ta2.o

ta2.o: ta2.c
	gcc $(CFLAGS) -c -o ta2.o ta2.c

clean:
	rm -f ta2 ta2.o