#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;


typedef struct link {
link *nextVirus;
virus *vir;
}link;

char sigFileName[256] = "signatures-L";

void SetSigFileName() {
    printf("Enter new signature file name: ");
    fgets(sigFileName, sizeof(sigFileName), stdin);
    sigFileName[strcspn(sigFileName, "\n")] = 0;  // Remove newline character
    printf("Signature file name set to: %s\n", sigFileName);  // Debug print
}

virus* readVirus(FILE* file) {
    int MAGIC_NUM_BYTES = 18;
   
    virus* v = (virus*)malloc(sizeof(virus));
    if (!v) {
        perror("Failed to allocate memory for virus");
        return NULL;
    }
    // Reads 18 bytes into the virus struct, which includes the signature size and the virus name
    if (fread(v, 1, MAGIC_NUM_BYTES, file) != MAGIC_NUM_BYTES) {
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
    //printf("Read virus: %s\n", v->virusName);  // Debug print
    return v;
}


void printVirus(virus* v) {
    if (v == NULL) return;
    printf("\n");
    printf("Virus name: %s\n", v->virusName);
    printf("Virus signature length: %d\n", v->SigSize);
    printf("Virus signature: \n");
    for (int i = 0; i < v->SigSize; ++i) {
        printf("%02X ", v->sig[i]);
        if ((i + 1) % 20 == 0) { // Insert a newline every 20 bytes
            printf("\n");
        }
    }
    if (v->SigSize % 20 != 0) { // If the last line didn't end in a newline, add one
        printf("\n");
    }
}

void freeVirus(virus* v) {
    if (v) {
        free(v->sig);
        free(v);
    }
}

void list_print(link *virus_list, FILE* output){
    link *current = virus_list;
    while (current != NULL) {
        printVirus(current->vir);
        current = current->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data) {
    link *newLink = (link*)malloc(sizeof(link));
    if (newLink == NULL) {
        perror("Failed to allocate memory for new link");
        return virus_list;
    }
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}

void list_free(link *virus_list) {
    link *current = virus_list;
    while (current != NULL) {
        link *next = current->nextVirus;
        freeVirus(current->vir);
        free(current);
        current = next;
    }
}

void detect_viruses() {
    printf("Not implemented\n");
}

void fix_file() {
    printf("Not implemented\n");
}

void optionPrint(int* choice,char *buffer){
    printf("\n");
    printf("0) Set signatures file name\n");
    printf("1) Load signatures\n");
    printf("2) Print signatures\n");
    printf("3) Detect viruses\n");
    printf("4) Fix file\n");
    printf("5) Quit\n");
    printf("Enter your choice: ");
    fgets(buffer, 256, stdin);
    sscanf(buffer, "%d",choice);
    printf("\n");
    
}

int main(int argc, char* argv[]) {
    int MAGIC_NUM_SIZE = 4;
    link *virus_list = NULL;
    int choice;
    char buffer[256];
    FILE* file;
    char magicNumber[MAGIC_NUM_SIZE];

    while(1){
        optionPrint(&choice,buffer);
        switch (choice){
            case 0:{
                SetSigFileName();
                break;
            }
            case 1:{                
                FILE* file = fopen(sigFileName, "rb");
                if (!file) {
                    perror("Failed to open signature file");
                    break;
                }
                // Read and check the magic number
                char magicNumber[MAGIC_NUM_SIZE];
                if (fread(magicNumber, 1, MAGIC_NUM_SIZE, file) != MAGIC_NUM_SIZE) {
                    fprintf(stderr, "Failed to read magic number\n");
                    fclose(file);
                }

                if (strncmp(magicNumber, "VIRL", MAGIC_NUM_SIZE) != 0 && strncmp(magicNumber, "VIRB", MAGIC_NUM_SIZE) != 0) {
                    fprintf(stderr, "Invalid magic number\n");
                    fclose(file);
                    return 1;
                }

                list_free(virus_list);
                virus_list = NULL;

                // Read and append viruses
                virus *v;
                //clock_t start =clock();                //Taken from https://stackoverflow.com/questions/31497531/what-is-the-type-of-stdchronohigh-resolution-clocknow-in-c11
                while ((v = readVirus(file)) != NULL) {
                    virus_list = list_append(virus_list, v);
                }
                // clock_t end = clock();
                // double duration = (double)(end - start) / CLOCKS_PER_SEC;
                // printf("Time taken to load signatures: %.6f seconds\n", duration);

                fclose(file);
                break;
            }
            case 2:{
                if (virus_list != NULL) {
                    list_print(virus_list, stdout);
                } else {
                    printf("No signatures loaded.\n");  // Debug print
                }
                break;
            }
            case 3:{
                detect_viruses();
                break;
            }
            case 4:{
                fix_file();
                break;
            }
            case 5:{
                list_free(virus_list);
                return 0;
            }
            default:{
                printf("Invalid choice, please try again.\n");
            }

        }
    }
    return 0;
}