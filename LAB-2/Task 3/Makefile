all: myshellTask3

myshellTask3: myshellTask3.o LineParser.o
	gcc -g -m32 -Wall -o myshellTask3 myshellTask3.o LineParser.o

myshellTask3.o: myshellTask3.c LineParser.h
	gcc -g -m32 -Wall -c myshellTask3.c

LineParser.o: LineParser.c LineParser.h
	gcc -g -m32 -Wall -c LineParser.c -o LineParser.o

clean:
	rm -f myshellTask3 myshellTask3.o LineParser.o
