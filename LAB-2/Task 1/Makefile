
all: myshell looper


myshell: LineParser.o myshell.o 
	 gcc LineParser.o myshell.o  -o myshell

looper: looper.o
	gcc looper.o -o looper


LineParser.o: LineParser.c LineParser.h
	 gcc -c LineParser.c -o LineParser.o


myshell.o: myshell.c LineParser.h
	 gcc -c myshell.c -o myshell.o


looper.o: looper.c
	gcc -c looper.c -o looper.o


clean:
	rm -f LineParser.o myshell.o looper.o myshell looper


.PHONY: all clean
