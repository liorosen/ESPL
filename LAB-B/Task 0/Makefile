all: BubbleSort

BubbleSort: BubbleSort.o
	gcc -g -m32 -Wall -o BubbleSort BubbleSort.o

BubbleSort.o: BubbleSort.c
	gcc -g -m32 -Wall -c BubbleSort.c

clean:
	rm -f BubbleSort BubbleSort.o
