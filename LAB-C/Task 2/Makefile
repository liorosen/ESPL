all:myshellpipeline2 

myshellpipeline2: myshellpipeline2.o LineParser.o
	gcc -g -m32 -Wall -o myshellpipeline2 myshellpipeline2.o LineParser.o

myshellpipeline2.o: myshellpipeline2.c LineParser.h
	gcc -g -m32 -Wall -c myshellpipeline2.c

LineParser.o: LineParser.c LineParser.h
	gcc -g -m32 -Wall -c LineParser.c -o LineParser.o

clean:
	rm -f myshellpipeline2 myshellpipeline2.o LineParser.o
