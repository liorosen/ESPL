#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

char sigFileName[256] = "signatures-L";

void SetSigFileName() {
    printf("Enter new signature file name: ");
    fgets(sigFileName, sizeof(sigFileName), stdin);
    sigFileName[strcspn(sigFileName, "\n")] = 0;  // Remove newline character
    printf("Signature file name set to: %s\n", sigFileName);  // Debug print
}

virus* readVirus(FILE* file) {
    virus* v = (virus*)malloc(sizeof(virus));
    if (!v) {
        perror("Failed to allocate memory for virus");
        return NULL;
    }
    // Reads 18 bytes into the virus struct, which includes the signature size and the virus name
    if (fread(v, 1, 18, file) != 18) {
        free(v);
        return NULL;
    }

    v->sig = (unsigned char*)malloc(v->SigSize);
    if (!v->sig) {
        perror("Failed to allocate memory for virus signature");
        free(v);
        return NULL;
    }

    // Reads the signature into the allocated memory
    if (fread(v->sig, 1, v->SigSize, file) != v->SigSize) {
        free(v->sig);
        free(v);
        return NULL;
    }

    return v;
}


void printVirus(virus* v) {
    if (v == NULL) return;
    printf("Virus name: %s\n", v->virusName);
    printf("Virus signature length: %d\n", v->SigSize);
    printf("Virus signature: ");
    for (int i = 0; i < v->SigSize; ++i) {
        printf("%02X ", v->sig[i]);
    }
    printf("\n");
}

void freeVirus(virus* v) {
    if (v) {
        free(v->sig);
        free(v);
    }
}


int main(int argc, char* argv[]) {
    SetSigFileName();

    printf("Attempting to open signature file: %s\n", sigFileName);  // Debug print
    FILE* file = fopen(sigFileName, "rb");
    if (!file) {
        perror("Failed to open signature file");
        return 1;
    }

    // Read and check the magic number
    char magicNumber[4];
    if (fread(magicNumber, 1, 4, file) != 4) {
        fprintf(stderr, "Failed to read magic number\n");
        fclose(file);
        return 1;
    }

    if (strncmp(magicNumber, "VIRL", 4) != 0 && strncmp(magicNumber, "VIRB", 4) != 0) {
        fprintf(stderr, "Invalid magic number\n");
        fclose(file);
        return 1;
    }

    // Read and print viruses one by one
    virus* v;
    while ((v = readVirus(file)) != NULL) {
        printVirus(v);
        freeVirus(v);
    }

    fclose(file);
    return 0;
}
