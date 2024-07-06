# Target to build all
all: myElf

# Linking myElf.o to create the myElf executable
myElf: myElf.o
	gcc -g -m32 -Wall -o myElf myElf.o

# Compiling myElf.c to create the myElf.o object file
myElf.o: myElf.c
	gcc -g -m32 -Wall -c myElf.c -o myElf.o

# Clean target to remove generated files
clean:
	rm -f myElf myElf.o
