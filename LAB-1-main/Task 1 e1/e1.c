#include <stdio.h>

int main(int argc, char **argv) {
    printf("Address of argv: %p\n", (void*)&argv);
    printf("Content of argv (addresses of argument strings):\n");

    // Loop through argv array to print each argument's address and value
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %p -> %s\n", i, (void*)&(argv[i]), argv[i]);
    }

    return 0;
}

/*What can you say about the memory location of the command-line arguments visible in main( )?

Answer: The command-line arguments in the main() function, represented by argv and its contents,
        are stored in the stack segment of memory.

        ~ Memory Location: The array of pointers argv, which holds the addresses of the command-line arguments,
                           is stored on the stack. 
        
        argv and the strings it points to , are all located on the stack.*/