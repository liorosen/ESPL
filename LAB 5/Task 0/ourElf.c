#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    int display_mode; // 0 for decimal, 1 for hexadecimal
    int fd1;
    int fd2;
    void* map_start1;
    void* map_start2;
    size_t file_size1;
    size_t file_size2;
} state;

state s = {0, "", 1, {0}, 0, 0, -1, -1, NULL, NULL, 0, 0}; // Initializing all the struct paramaters by order of creation.

// Switches the debug mode on or off
void toggle_debug_mode(state* s) {
    s->debug_mode = !s->debug_mode;
    printf("Debug flag now %s\n", s->debug_mode ? "on" : "off");
}

void print_elf_header(Elf32_Ehdr* header) {
    printf("Magic: %.3s\n", header->e_ident + 1);
    printf("Data encoding: %d\n", header->e_ident[EI_DATA]);
    printf("Entry point: %#08x\n", header->e_entry);
    printf("Section header table offset: %d\n", header->e_shoff);
    printf("Number of section header entries: %d\n", header->e_shnum);
    printf("Size of section header entry: %d\n", header->e_shentsize);
    printf("Program header table offset: %d\n", header->e_phoff);
    printf("Number of program header entries: %d\n", header->e_phnum);
    printf("Size of program header entry: %d\n", header->e_phentsize);
}

void mangagingMultipleFiles(state* s, int fd, struct stat sb, void* map_start) {
    if (s->fd1 == -1) {
        s->fd1 = fd;
        s->map_start1 = map_start;
        s->file_size1 = sb.st_size;
    } else if (s->fd2 == -1) {
        s->fd2 = fd;
        s->map_start2 = map_start;
        s->file_size2 = sb.st_size;
    } else {
        printf("Already examining two ELF files\n");
        munmap(map_start, sb.st_size);
        close(fd);
        return;
    }
}

bool map_and_validate_elf_file(int fd, struct stat sb, void** map_start) {
    *map_start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    /* Internal chooses the address at which to create the mapping, Specifies the length of the mapping,
       PROT_READ- indicates that the pages may be read , MAP_PRIVATE creates a private copy-on-write mapping,
       file descriptor of the file to be mapped, specifies the offset in the file where the mapping starts.*/

    if (*map_start == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return false;
    }

    Elf32_Ehdr* header = (Elf32_Ehdr*) *map_start;
    // Checking the magic number ensures that the file being processed is indeed an ELF file and not some other type of file
    if (memcmp(header->e_ident, ELFMAG, SELFMAG) != 0) { // Compares the first SELFMAG (4) bytes of the e_ident array in the ELF header with the ELFMAG magic number. 
        printf("Not an ELF file\n");
        munmap(*map_start, sb.st_size); // Releases the memory region that was previously mapped , and ensures that the virtual memory allocated for the mapping is freed and can be reused.
        close(fd); // Close the file descriptor
        return false;
    }

    return true;
}

void errChecks(int fd, struct stat *sb) {
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    // Initialize the struct stat object before using it
    memset(sb, 0, sizeof(struct stat));
    
    // Get information about the file referred to by the file descriptor fd
    if (fstat(fd, sb) == -1) {
        perror("Error getting file size");
        close(fd);
        return;
    }
}

void examine_elf_file(state* s) {
    char file_name[128];
    int fd;
    struct stat sb;
    void* map_start;

    printf("Enter ELF file name: ");
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0';

    fd = open(file_name, O_RDONLY);
    
    errChecks(fd, &sb);

    // Call the helper function to map and validate the ELF file
    if (!map_and_validate_elf_file(fd, sb, &map_start)) {
        return; // If the file is not a valid ELF file, return early
    }

    // Call the function to manage multiple ELF files
    mangagingMultipleFiles(s, fd, sb, map_start);

    print_elf_header((Elf32_Ehdr*) map_start);
}


void stub_function(state* s) {
    printf("Not implemented yet\n");
}

void quit(state* s) {
    if (s->fd1 != -1) {
        munmap(s->map_start1, s->file_size1);
        close(s->fd1);
    }
    if (s->fd2 != -1) {
        munmap(s->map_start2, s->file_size2);
        close(s->fd2);
    }
    if (s->debug_mode) {
        fprintf(stderr, "quitting\n");
    }
    exit(0);
}

typedef void (*menu_function)(state* s);

menu_function menu_functions[] = {
    toggle_debug_mode,
    examine_elf_file,
    stub_function, // Print Section Names
    stub_function, // Print Symbols
    stub_function, // Check Files for Merge
    stub_function, // Merge ELF Files
    quit,
    NULL
};

const char* menu[] = {
    "0-Toggle Debug Mode",
    "1-Examine ELF File",
    "2-Print Section Names",
    "3-Print Symbols",
    "4-Check Files for Merge",
    "5-Merge ELF Files",
    "6-Quit",
    NULL
};

void print_menu() {
    printf("\n");
    for (int i = 0; menu[i] != NULL; i++) {
        printf("%s\n", menu[i]);
    }
}

int main() {
    int choice;

    while (1) {
        if (s.debug_mode) {
            // Print debug info if needed
        }
        print_menu();
        printf("> ");
        scanf("%d", &choice);
        getchar(); // Consume the newline character after scanf
        if (choice >= 0 && choice <= 6) {
            menu_functions[choice](&s);
        } else {
            printf("Invalid choice\n");
        }
    }
    return 0;
}

