# Makefile for building the encoder project

# Default target to build everything
all: task1

# Link the object files to create the final executable
task1: start.o util.o encoder.o
	ld -m elf_i386 start.o encoder.o util.o -o task1

# Assemble the start.s file to create start.o
start.o: start.s 
	nasm -f elf32 start.s -o start.o

# Compile the util.c file to create util.o
util.o: util.c util.h
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

# Assemble the encoder.s file to create encoder.o
encoder.o: encoder.s 
	nasm -f elf32 encoder.s -o encoder.o

# Phony target to clean up the build directory
.PHONY: clean 

# Clean up the build directory
clean: 
	rm -f *.o task1
