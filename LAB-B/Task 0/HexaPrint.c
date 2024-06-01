#include <stdio.h>
#include <stdlib.h>

void PrintHex(const unsigned char *buffer, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    //Tries to open file
    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }
    
    fseek(file, 0, SEEK_END);     // Move to the end of the file
    long filesize = ftell(file);  // Get the size of the file
    fseek(file, 0, SEEK_SET);     // Move back to the beginning of the file

    // Allocate memory for the file contents
    unsigned char *buffer = (unsigned char *)malloc(filesize);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    // Read the file contents into memory
    /* fread function info from https://en.cppreference.com/w/c/io/fread:
        buffer	-	pointer to the array where the read objects are stored- a pointer to a block of memory.
        size	-	size of each object in bytes
        count	-	the number of the objects to be read
        stream	-	A pointer to a FILE object that specifies an input stream.*/
    size_t read_size = fread(buffer, 1, filesize, file);
    if (read_size != filesize) {
        perror("Failed to read file");
        free(buffer);
        fclose(file);
        return 1;
    }

    // Print the contents in hexadecimal format
    PrintHex(buffer, read_size);

    // Clean up
    free(buffer);
    fclose(file);

    return 0;
}