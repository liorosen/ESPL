#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[1024];  // Buffer for input
int buf_pos = 0;    // Current position in buffer

char my_get(char c) {
     
    if((c = fgetc(stdin)) == '\n'){return 0;}
    return c;  // Get char from user and return it
}

void reset_input(char *buf) {
    for(int i = 0 ; i < 5 ; i++){
        buf[i] = 0;
    }
}

char* map(char *array, int array_length, char (*f) (char)) {
    int i;
    /* Allocate memory for the new array*/
    char* mapped_array = (char*) malloc(array_length * sizeof(char));
    if (mapped_array == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* Apply the function 'f' to each element of the input array*/
    for ( i = 0; i < array_length ; i++) {
        mapped_array[i] = f(array[i]);
    }

    return mapped_array;
}

char changeChar(char c) {
  c += 10;
  return c;
}


// char my_get(char c){
// /* Ignores c, reads and returns a character from stdin using fgetc. */
//     return fgetc(stdin);
// }


char cprt(char c){
    if (c == 0){
        return 0;
    }
    /* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */
    if( c >= 0x20 && c <= 0x7E){
        printf("%c\n", c);
    }
    else{printf(".\n");}
    return c;
}

char encrypt(char c){
    if (c == 0){
        return 0;
    }
    /* Gets a char c. If c is between 0x20 and 0x4E add 0x20 to its value and return it. Otherwise return c unchanged */
     if( c >= 0x20 && c <= 0x4E){
        c += 0x20;
        return c;
     }
     return c;
}
char decrypt(char c){
    if (c == 0){
        return 0;
    }
    /* Gets a char c and returns its decrypted form subtractng 0x20 from its value. But if c was not between 0x40 and 0x7E it is returned unchanged */
    if( c >= 0x20 && c <= 0x7E){
        c -= 0x20;
        return c;
    }
    return c;
}

char xoprt(char c){
    if (c == 0){
        return 0;
    }
    /* xoprt prints the value of c in a hexadecimal representation, then in octal representation, followed by a new line, and returns c unchanged. */
    
    printf("%x          ", c);
    printf("%o\n", c);
    return c;
}


struct fun_desc { 
  char *name; 
  char (*fun)(char); 
};

void helper(int options, int j,  struct fun_desc menu[]){
    int i = j;
    printf("Select operation from the following menu:\n");
        for ( i = 0; menu[i].name; i++) {
            printf("%i) %s\n", i, menu[i].name);
        }
    printf("Option: ");
}



int main() {

    char carray[5] = {0};
    int options;
    int i = 0;
    char *newArray;
    char input[1024]; 
    struct fun_desc menu[] = {{"Get String" , my_get},{"Print string" , cprt},{"Print Hex" , xoprt}, { "Encrypt", encrypt},{"Decrypt" , decrypt}
    ,{ NULL, NULL } }; 
        
    while (1) {
        helper(options, i, menu);

        
        /* Prevents buffer overflow*/
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error or EOF encountered.\n");
            break;
        }
        /*If user press Enter */
        if (sscanf(input, "%d", &options) != 1 ) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
                
        
        if ( options < 0 || menu[options].name == NULL || options >= (sizeof(menu) / sizeof(menu[0]) - 1) || !menu[options].name) {
            printf("Not within bounds\n");
            exit(EXIT_FAILURE);

        } else {
            if( options == 0){
                reset_input(carray);
            }
            printf("Within bounds\n");
            newArray = map(carray, 5, menu[options].fun);
            strncpy(carray, newArray, 5);
            free(newArray);
            printf("Done.\n");
            
        }
    }
    return 0;
}
   
