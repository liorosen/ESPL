#include <stdio.h>
#include <stdlib.h>

char* map(char *array, int array_length, char (*f) (char)) {
    /* Allocate memory for the new array*/
    char* mapped_array = (char*) malloc(array_length * sizeof(char));
    if (mapped_array == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* Apply the function 'f' to each element of the input array*/
    for (int i = 0; i < array_length; i++) {
        mapped_array[i] = f(array[i]);
    }

    return mapped_array;
}

char changeChar(char c) {
  c += 10;
  return c;
}


char my_get(char c){
/* Ignores c, reads and returns a character from stdin using fgetc. */
    return fgetc(stdin);
}


char cprt(char c){
    /* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */
    if( c >= 0x20 && c <= 0x7E){
        printf("%c\n", c);
    }
    else{printf(".\n");}
    return c;
}

char encrypt(char c){
    /* Gets a char c. If c is between 0x20 and 0x4E add 0x20 to its value and return it. Otherwise return c unchanged */
     if( c >= 0x20 && c <= 0x4E){
        c += 0x20;
        return c;
     }
     return c;
}
char decrypt(char c){
    /* Gets a char c and returns its decrypted form subtractng 0x20 from its value. But if c was not between 0x40 and 0x7E it is returned unchanged */
    if( c >= 0x20 && c <= 0x7E){
        c -= 0x20;
        return c;
    }
    return c;
}

char xoprt(char c){
    /* xoprt prints the value of c in a hexadecimal representation, then in octal representation, followed by a new line, and returns c unchanged. */
    printf("Octal value is: %o\n",c);
    printf("Hexadecimal value is (Alphabet in small letters): %x\n",c);
    return c;
}

int main(int argc, char **argv){
   /* TODO: Test your code */
  int base_len = 5;
  char arr1[base_len];
  char* arr2 = map(arr1, base_len, my_get); 
  printf("\n");
  char* arr3 = map(arr2, base_len, cprt); 
  printf("\n");
  char* arr4 = map(arr3, base_len, xoprt); 
  printf("\n");
  char* arr5 = map(arr4, base_len, encrypt); 
  printf("\n");
  
  free(arr2); 
  free(arr3); 
  free(arr4); 
  free(arr5); 
  
}

