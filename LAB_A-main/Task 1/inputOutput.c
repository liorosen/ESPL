#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1
#define NODEBUG 0

unsigned int is_digit(char character){
    if(character >= '0' && character <='9' )
        return 1;
    return 0; 
}

unsigned int is_lower(char character){
    if(character >= 'a' && character <='z' )
        return 1;
    return 0; 
}

/*unsigned int is_upper(char character){
    if(character >= 'A' && character <='Z' )
        return 1;
    return 0; 
}*/

unsigned int strlength(const char *string){
    if(!string)
        return 0;

    int length = 0;
    while(string[length++]);
    return length-1;
}

void encoderHelper (int *index, int *ch, int add, const char *key,int key_length){
    if (*ch == '\n' ) {
        return; // Skip processing newline characters.
    }
    if ((is_lower(*ch) || is_digit(*ch)/*|| is_upper(*ch)*/)  && key_length > 0) {
                int shift = key[*index] - '0';
                if (!add){
                    shift = -shift;
                }
                if (is_lower(*ch)) {
                    *ch = 'a' + (26 + (*ch - 'a' + shift) % 26) % 26;
                } else if (is_digit(*ch)) {
                    *ch = '0' + (10 + (*ch - '0' + shift) % 10) % 10;
                }/* else if (is_upper(*ch)) {
                    *ch = 'A' + (26 + (*ch - 'A' + shift) % 26) % 26;
                }*/
                *index = (*index + 1) % key_length;
            }
}
 void encode_and_output(FILE *infile, FILE *outfile, const char *key, int add, int debug) {
        int ch = 0;
        int index = 0;
        int key_length = strlength(key);
        while ((ch = fgetc(infile)) != EOF) {
            
            if (debug && ch != '\n') {
                fprintf(stderr, "Read character: %c\n", ch);
            }
            encoderHelper (&index,&ch, add, key,key_length);
            fputc(ch, outfile);
            if (debug && ch != '\n') {
                fprintf(stderr, "Output character: %c\n", ch);
            }
        }
        
  }

void debugerfunc(int argc, char *argv[], int i , int *debugMode){
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-D", 2) == 0) {
            *debugMode = NODEBUG; 
        } else if (strncmp(argv[i], "+D", 2) == 0) {
            *debugMode = DEBUG;  
        } else if (debugMode) {
            fprintf(stderr, "Argument %d: %s\n", i, argv[i]);
        }
    }
}

void searchFore(int argc, char *argv[], int i, int *add , char **key){
    /*loop looks for +e or -e*/
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "+e", (unsigned int)2) == 0 || strncmp(argv[i], "-e", (unsigned int)2) == 0) {
            *key = argv[i] + 2;
            *add = (argv[i][0] == '+') ? 1 : 0;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    int debugMode = DEBUG;  
    FILE *infile = stdin;
    FILE *outfile = stdout;
    int add = 1; 
    char *key = NULL;
    int i = 0;
    debugerfunc( argc, argv, i, &debugMode);

    
    for (i = 1; i < argc; i++) {
        /* Loop to check for file input = -I */
        if (strncmp(argv[i], "-I", 2) == 0) {
            infile = fopen(argv[i] + 2, "r");
            if (infile == NULL) {
                fprintf(stderr, "Error opening input file %s\n", argv[i] + 2);
                return 1;  // Equivalent to EXIT_FAILURE
            }
        /* Loop to check for file output flags = -O */    
        } else if (strncmp(argv[i], "-O", 2) == 0) {
            outfile = fopen(argv[i] + 2, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Error opening output file %s\n", argv[i] + 2);
                if (infile != stdin) {
                    fclose(infile);
                }
                return 1;
            }
        }
    }

    searchFore(argc, argv, i, &add , &key);
    if (key == NULL) {
        fprintf(stderr, "No encoding key provided.\n");
        return 1;
    }
    encode_and_output(infile, outfile, key,add, debugMode);
   
    /* Close files if not standard input/output*/
    if (infile != stdin) {
        fclose(infile);
    }
    if (outfile != stdout) {
        fclose(outfile);
    }

    return 0;  // Equivalent to EXIT_SUCCESS
}
