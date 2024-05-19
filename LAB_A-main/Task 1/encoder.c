#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to process and encode the input
void process_input(FILE *infile, FILE *outfile, const char *key, int add) {
    int ch, index = 0;
    int key_length = strlen(key);

    while ((ch = fgetc(infile)) != EOF) {
        if (islower(ch) || isdigit(ch)) {
            int shift = key[index] - '0';
            if (!add) {
                shift = -shift;
            }

            if (islower(ch)) {
                ch = 'a' + (ch - 'a' + shift + 26) % 26;
            } else if (isdigit(ch)) {
                ch = '0' + (ch - '0' + shift + 10) % 10;
            }

            index = (index + 1) % key_length;
        }

        fputc(ch, outfile);
    }
}

int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    char key[100] = {0};
    int add = 1;  // Default to addition
    int i = 1;
    for (; i < argc; i++) {
        if (strcmp(argv[i], "-I") == 0 && i + 1 < argc) {
            infile = fopen(argv[++i], "r");
            if (!infile) {
                perror("Failed to open input file");
                return 1;
            }
        } else if (strcmp(argv[i], "-O") == 0 && i + 1 < argc) {
            outfile = fopen(argv[++i], "w");
            if (!outfile) {
                perror("Failed to open output file");
                return 1;
            }
        } else if (strncmp(argv[i], "+e", 2) == 0) {
            strcpy(key, argv[i] + 2);
            add = 1;
        } else if (strncmp(argv[i], "-e", 2) == 0) {
            strcpy(key, argv[i] + 2);
            add = 0;
        }
    }

    process_input(infile, outfile, key, add);

    if (infile != stdin) fclose(infile);
    if (outfile != stdout) fclose(outfile);

    return 0;
}
