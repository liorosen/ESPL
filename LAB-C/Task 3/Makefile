all: myshellpipeline3

myshellpipeline3: myshellpipeline3.o LineParser.o
	gcc -g -m32 -Wall -o myshellpipeline3 myshellpipeline3.o LineParser.o

myshellpipeline3.o: myshellpipeline3.c LineParser.h
	gcc -g -m32 -Wall -c myshellpipeline3.c

LineParser.o: LineParser.c LineParser.h
	gcc -g -m32 -Wall -c LineParser.c -o LineParser.o

clean:
	rm -f myshellpipeline3 myshellpipeline3.o LineParser.o
