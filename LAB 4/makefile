all: hexeditplus

hexeditplus: hexeditplus.o 
	gcc -g -m32 -Wall -o hexeditplus hexeditplus.o 

hexeditplus.o: hexeditplus.c 
	gcc -g -m32 -Wall -c hexeditplus.c

clean:
	rm -f hexeditplus hexeditplus.o 
