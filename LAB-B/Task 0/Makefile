all: BubbleSort hexaPrint

BubbleSort: BubbleSort.o
	gcc -g -m32 -o BubbleSort BubbleSort.o

BubbleSort.o: BubbleSort.c
	gcc -g -m32 -c BubbleSort.c

hexaPrint: HexaPrint.o
	gcc -g -m32 -o hexaPrint HexaPrint.o

HexaPrint.o: HexaPrint.c
	gcc -g -m32 -c HexaPrint.c

clean:
	rm -f BubbleSort BubbleSort.o  hexaPrint hexaPrint.o
