#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 10000

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;


typedef struct link {
    struct link *nextVirus;
    virus *vir;
}link;

char sigFileName[256] = "signatures-L";
char suspectFileName[256];
link *virus_list = NULL;

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

// Modifies the first byte of the virus signature to the RET instruction
void neutralize_virus(const char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "r+b");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    fseek(file, signatureOffset, SEEK_SET);
    unsigned char retInstruction = 0xC3;
    fwrite(&retInstruction, 1, 1, file);
    fclose(file);
}

void scanBuffer(virus *v, char *buffer, unsigned int size, int fix,int *virusFound) {
    for (unsigned int i = 0; i < size - v->SigSize + 1; i++) {
        if (memcmp(buffer + i, v->sig, v->SigSize) == 0) {
            *virusFound = 1;
            printf("Starting byte location: %u\n", i);
            printf("Virus name: %s\n", v->virusName);
            printf("Virus signature size: %d\n", v->SigSize);
            
            if (fix) {
                neutralize_virus(suspectFileName, i);
                printf("Virus neutralized at byte location: %u\n", i);
            }
        }
    }
        
}

void detect_virus_helper(char *buffer, unsigned int size, int fix) {
    int virusFound = 0;
    link *current = virus_list;
    while (current != NULL) {
        virus *v = current->vir;
        scanBuffer(v, buffer, size, fix, &virusFound);
        current = current->nextVirus;
    }
    if (!virusFound && fix) {
        printf("No viruses found to neutralize.\n");
    }
    if (!virusFound && !fix) {
        printf("No viruses found.\n");
    }
}

// Opens the file in the appropriate mode
void fileBufferOpener(int fix){
    FILE *file = fopen(suspectFileName, fix ? "r+b" : "rb");
    if (!file) {
        perror("Failed to open suspected file");
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t size = fread(buffer, 1, BUFFER_SIZE, file);
    if (size == 0) {
        fprintf(stderr, "Failed to read suspected file\n");
        fclose(file);
        return;
    }

    detect_virus_helper(buffer, size, fix);

    fclose(file);
}

void detect_or_fix(int fix) {
    if (strlen(suspectFileName) == 0) {
        printf("Enter the suspected file name: ");
        fgets(suspectFileName, sizeof(suspectFileName), stdin);
        suspectFileName[strcspn(suspectFileName, "\n")] = 0;  // Remove newline character
    }
    
    fileBufferOpener(fix);
    suspectFileName[0] = '\0'; // Reset suspectFileName for next use
    printf("\n");
}

void fix_file() {
    detect_or_fix(1);
}

void detect_virus() {
    detect_or_fix(0);
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

void loadSignatures(int MAGIC_NUM_SIZE){
    FILE* file = fopen(sigFileName, "rb");
    if (!file) {
        perror("Failed to open signature file");
        return;
    }
    // Read and check the magic number
    char magicNumber[MAGIC_NUM_SIZE];
    if (fread(magicNumber, 1, MAGIC_NUM_SIZE, file) != MAGIC_NUM_SIZE) {
        fprintf(stderr, "Failed to read magic number\n");
        fclose(file);
    }

    //Check if the first 4 characters of magicNumber are VIRL or VIRB
    if (strncmp(magicNumber, "VIRL", MAGIC_NUM_SIZE) != 0 && strncmp(magicNumber, "VIRB", MAGIC_NUM_SIZE) != 0) {
        fprintf(stderr, "Invalid magic number\n");
        fclose(file);
        return ;
    }

    list_free(virus_list);
    virus_list = NULL;

    // Read and append viruses
    virus *v;
    while ((v = readVirus(file)) != NULL) {
        virus_list = list_append(virus_list, v);
    }

    fclose(file);
    printf("Loaded succeeded\n");
}

int mainCases(int choice , char *buffer ,int MAGIC_NUM_SIZE ){
    while(1){
        optionPrint(&choice,buffer);
        switch (choice){
            case 0:
                {SetSigFileName();
                printf("\n");
                break;}
            
            case 1:                
                {loadSignatures( MAGIC_NUM_SIZE);
                break;}
            
            case 2:
                {if (virus_list != NULL) {
                    list_print(virus_list, stdout);
                } else {
                    printf("No signatures loaded.\n");  // Debug print
                }
                printf("\n");
                break;}
            
            case 3:
                {detect_virus();
                break;}
            

            case 4:
                {fix_file();
                break;}
            
            case 5:
                {list_free(virus_list);
                return 0;}
            
            default:
                {printf("Invalid choice, please try again.\n");}
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int MAGIC_NUM_SIZE = 4;
    int choice;
    char buffer[256];
    FILE* file;
    char magicNumber[MAGIC_NUM_SIZE];

    return mainCases(choice , buffer , MAGIC_NUM_SIZE );
}