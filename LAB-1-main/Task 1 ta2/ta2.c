#include <stdio.h>

int main() {
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3];

    // Print the addresses of each array element
    for (int i = 0; i < 3; i++) {
        printf("iarray[%d]: %p\n", i, (void*)&iarray[i]);
        printf("farray[%d]: %p\n", i, (void*)&farray[i]);
        printf("darray[%d]: %p\n", i, (void*)&darray[i]);
        printf("carray[%d]: %p\n", i, (void*)&carray[i]);
    }

    // Print the addresses using pointer arithmetic
    printf("iarray: %p, iarray+1: %p\n", (void*)iarray, (void*)(iarray+1));
    printf("farray: %p, farray+1: %p\n", (void*)farray, (void*)(farray+1));
    printf("darray: %p, darray+1: %p\n", (void*)darray, (void*)(darray+1));
    printf("carray: %p, carray+1: %p\n", (void*)carray, (void*)(carray+1));

    return 0;
}

/* 1. What can you say about the behavior of the '+' operator?
   2. Given the results, explain to the TA the memory layout of arrays.

   Answer:   iarray[0]: 0xffcc36e8 , farray[0]: 0xffcc36f4, darray[0]: 0xffcc3700, carray[0]: 0xffcc3719
        iarray[1]: 0xffcc36ec , farray[1]: 0xffcc36f8, darray[1]: 0xffcc3708, carray[1]: 0xffcc371a
        iarray[2]: 0xffcc36f0 , farray[2]: 0xffcc36fc, darray[2]: 0xffcc3710, carray[2]: 0xffcc371b
        
        iarray: 0xffcc36e8, iarray+1: 0xffcc36ec
        farray: 0xffcc36f4, farray+1: 0xffcc36f8
        darray: 0xffcc3700, darray+1: 0xffcc3708
        carray: 0xffcc3719, carray+1: 0xffcc371a

        Integer Array (int iarray[3]):    The size of an int is typically 4 bytes.
             Therefore, when you do iarray + 1, the address increases by 4 bytes.

        Float Array   (float farray[3]):  The size of a float is also typically 4 bytes.
             Thus, farray + 1 will also show an increase of 4 bytes.

        Double Array  (double darray[3]): The size of a double is usually 8 bytes.
             As a result, darray + 1 will increase the address by 8 bytes.

        Char Array    (char carray[3]):   The size of a char is 1 byte, 
            so carray + 1 will increase the address by just 1 byte.

        The '+' operator when used with pointers increments the pointer by the size of the type it points to.
        This is why the memory address increments by the size of an element of the array's type.
        Pointer making it easy to iterate through arrays regardless of the element size.
    

T1c - Distances
Understand and explain to the TA the purpose of the distances printed in the point_at function.
Where is each memory address allocated and what does it have to do with the printed distance? Given the results, explain to the TA the memory layout of arrays. 

Relative Memory Layout: The distance between pointers can reveal how variables (e.g., array elements or standalone variables) are positioned relative to each other in memory—whether they are close together (indicating they may be part of the same data structure) or far apart (indicating different memory segments or unrelated data structures).
Memory Segmentation: Based on the address values, one can determine if the addresses belong to the stack, the heap, or global/static memory areas:
Stack: Local variables within a function reside here.
Heap: Dynamically allocated memory (via malloc, calloc, etc.) is found here.
Data Segment (Global/Static): Global and static variables are stored here.

~ Arrays in C are stored in contiguous blocks of memory. If array[0] is at address addr,
  then array[1] is at addr + sizeof(array element), and so on. 
  This contiguous nature allows for efficient indexing and iteration.

~ Adding 1 to a pointer to an array element moves the pointer by the size of one element of the array's
  type forward in memory.
  The difference between two pointers within the same array, therefore,
  gives the number of elements between them, multiplied by the size of each element.
  For Example : The distance between &ints[0] and &ints[1] is 4 bytes, illustrating the size of an int. 
                The distance between &doubles[0] and &doubles[1] is 8 bytes, matching the size of a double*/