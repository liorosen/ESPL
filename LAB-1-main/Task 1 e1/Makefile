CFLAGS=-g -Wall -m32

all: e1

e1: e1.o
	gcc $(CFLAGS) -o e1 e1.o

e1.o: e1.c
	gcc $(CFLAGS) -c -o e1.o e1.c

clean:
	rm -f e1 e1.o