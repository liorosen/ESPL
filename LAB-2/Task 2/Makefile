# Default target
all: myshellTask2 looper

# Rule for building the executable
myshellTask2: LineParser.o myshellTask2.o 
	 gcc LineParser.o myshellTask2.o  -o myshellTask2

looper: looper.o
	gcc looper.o -o looper

# Rule for building LineParser.o
LineParser.o: LineParser.c LineParser.h
	 gcc -c LineParser.c -o LineParser.o

# Rule for building myshellTask2.o
myshellTask2.o: myshellTask2.c LineParser.h
	 gcc -c myshellTask2.c -o myshellTask2.o

# Rule for building looper.o
looper.o: looper.c
	gcc -c looper.c -o looper.o

# Rule for cleaning up generated files
clean:
	rm -f LineParser.o myshellTask2.o looper.o myshellTask2 looper

# Phony targets
.PHONY: all clean
