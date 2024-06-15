all: myshellpipeline

myshellpipeline: myshellpipeline.o LineParser.o
	gcc -g -m32 -Wall -o myshellpipeline myshellpipeline.o LineParser.o

myshellpipeline.o: myshellpipeline.c LineParser.h
	gcc -g -m32 -Wall -c myshellpipeline.c

LineParser.o: LineParser.c LineParser.h
	gcc -g -m32 -Wall -c LineParser.c -o LineParser.o

clean:
	rm -f myshellpipeline myshellpipeline.o LineParser.o
