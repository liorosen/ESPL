all: main

main: main.o print_multi.o
	gcc -m32 -o main main.o print_multi.o -no-pie

main.o: main.c
	gcc -m32 -c -o main.o main.c

print_multi.o: print_multi.s
	nasm -f elf32 -o print_multi.o print_multi.s

clean:
	rm -f main main.o print_multi.o
