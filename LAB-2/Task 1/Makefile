# Default target
all: myshell looper

# Rule for building the executable
myshell: LineParser.o myshell.o 
	 gcc LineParser.o myshell.o  -o myshell

looper: looper.o
	gcc looper.o -o looper

# Rule for building LineParser.o
LineParser.o: LineParser.c LineParser.h
	 gcc -c LineParser.c -o LineParser.o

# Rule for building myshell.o
myshell.o: myshell.c LineParser.h
	 gcc -c myshell.c -o myshell.o

# Rule for building looper.o
looper.o: looper.c
	gcc -c looper.c -o looper.o

# Rule for cleaning up generated files
clean:
	rm -f LineParser.o myshell.o looper.o myshell looper

# Phony targets
.PHONY: all clean
