# Target to build all
all: ourElf

# Linking ourElf.o to create the ourElf executable
ourElf: ourElf.o
	gcc -g -m32 -Wall -o ourElf ourElf.o

# Compiling ourElf.c to create the ourElf.o object file
ourElf.o: ourElf.c
	gcc -g -m32 -Wall -c ourElf.c -o ourElf.o

# Clean target to remove generated files
clean:
	rm -f ourElf ourElf.o
