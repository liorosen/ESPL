all: clean assemble compile link

assemble:
	nasm -f elf32 start.s -o start.o

	

compile:
	gcc -m32 -Wall -ansi -c util.c -o util.o
	gcc -m32 -Wall -ansi -c main.c -o main.o

	
	

link:
	ld -m elf_i386 start.o main.o util.o -o task2B




clean:
	rm -f start.o util.o task1B
