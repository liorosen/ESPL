CFLAGS=-g -Wall -m32

all: menu

menu: menu.o
	gcc $(CFLAGS) -o menu menu.o

menu.o: menu.c
	gcc $(CFLAGS) -c -o menu.o menu.c

clean:
	rm -f menu menu.o
