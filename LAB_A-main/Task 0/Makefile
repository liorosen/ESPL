CFLAGS = -m32 -g -Wall
ASMFLAGS = -f elf32 -g

TARGET = main
OBJS = main.o numbers.o add.o

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

numbers.o: numbers.c
	gcc $(CFLAGS) -c numbers.c -o numbers.o

add.o: add.s
	nasm $(ASMFLAGS) -o add.o add.s
