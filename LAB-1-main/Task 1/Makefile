CFLAGS=-g -Wall -m32 

all: addresses

addresses: addresses.o
	gcc $(CFLAGS) -o addresses addresses.o

addresses.o: addresses.c
	gcc $(CFLAGS) -c -o addresses.o addresses.c

clean:
	rm -f addresses addresses.o