#include <stdio.h>

struct multi {
    unsigned char size;
    unsigned char num[255];  // Adjust max size as needed
};

extern void print_multi(struct multi *p);

// Initialize global struct
struct multi x_struct = {
    .size = 5,
    .num = {0xaa, 1, 2, 0x44, 0x4f}
};

int main() {
    print_multi(&x_struct);
    return 0;
}
