CFLAGS=-g -Wall 

all: count-words

count-words: count-words.o
	gcc $(CFLAGS) -o count-words count-words.o

count-words.o: count-words.c
	gcc $(CFLAGS) -c -o count-words.o count-words.c

clean:
	rm -f count-words count-words.o
