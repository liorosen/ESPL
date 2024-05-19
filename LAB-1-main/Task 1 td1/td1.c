#include <stdio.h>

int main() {
    /* Declare and initialize the arrays*/
    int iarray2[] = {1, 2, 3};
    char carray2[] = {'a', 'b', 'c'};

    /* Declare an uninitialized pointer*/
    int *p;

    /* Declare pointers and initialize them to point to the first elements of the arrays*/
    int* iarray2Ptr = iarray2;
    char* carray2Ptr = carray2;

    /* Print all values using pointers for the integer array*/
    printf("Integer array values: \n");
    for (int i = 0; i < 3; i++) {
        printf("%d ");
        printf(*(iarray2Ptr + i)); 
    }
    printf("\n");

    /* Print all values using pointers for the char array*/
    printf("Character array values: \n");
    for (int i = 0; i < 3; i++) {
        printf("%c "); 
        printf(*(carray2Ptr + i));
    }
    printf("\n");

    
    /* Print the value of the uninitialized pointer*/
    printf("Uninitialized pointer value: %p\n");
    printf((void*)p);

    return 0;
}


