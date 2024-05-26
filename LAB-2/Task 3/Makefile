all: myshellTask2

myshellTask2: myshellTask2.o
	sudo apt update-Wall -o myshellTask2 myshellTask2.o

myshellTask2.o: myshellTask2.c LineParser.h
	gcc -g -m32 -Wall -c myshellTask2.c

clean:
	rm -f myshellTask2 *.o
