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
    char file_name1[128]; // Added to store file name for fd1
    char file_name2[128]; // Added to store file name for fd2
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

state s = {0, "", "", 1, {0}, 0, 0, -1, -1, NULL, NULL, 0, 0}; // Initializing all struct parameters by order of creation.

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
    *map_start = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
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

bool err_checks(int fd, struct stat *sb) {
    if (fd == -1) {
        perror("Error opening file");
        return false;
    }

    memset(sb, 0, sizeof(struct stat));
    if (fstat(fd, sb) == -1) {
        perror("Error getting file size");
        close(fd);
        return false;
    }
    return true;
}

void examine_elf_file(state* s) {
    if (s->fd1 != -1 && s->fd2 != -1) {
        printf("Already examining two ELF files\n");
        return;
    }

    char file_name[128];
    int fd;
    struct stat sb;
    void* map_start;

    printf("Enter ELF file name: ");
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0';

    fd = open(file_name, O_RDONLY);
    if (!err_checks(fd, &sb)) {
        printf("Error opening file: %s\n", file_name);
        return;
    }
    if (!map_and_validate_elf_file(fd, sb, &map_start)) {
        return;
    }

    Elf32_Ehdr* header = (Elf32_Ehdr*) map_start;

    mangagingMultipleFiles(s, fd, sb, map_start);
    if (fd == s->fd1) {
        strcpy(s->file_name1, file_name); // Store file name for fd1
    } else if (fd == s->fd2) {
        strcpy(s->file_name2, file_name); // Store file name for fd2
    }
    print_elf_header(header);
}


const char* get_section_type(uint32_t type) {
    // Reference from here : https://refspecs.linuxfoundation.org/LSB_2.1.0/LSB-Core-generic/LSB-Core-generic/elftypes.html
    //  Each section has a specific type that indicates what kind of data it contains
    switch (type) {
        case SHT_NULL: return "NULL";           // Used as a placeholder for an invalid section or to mark the end of the section header array.
        case SHT_PROGBITS: return "PROGBITS";   // Contains application-specific information, such as the .text (code) and .data (initialized data) sections
        case SHT_SYMTAB: return "SYMTAB";       // Contains information about functions and global variables in the program.
        case SHT_STRTAB: return "STRTAB";       // Used for the .strtab section, which holds strings referenced by the symbol table.
        case SHT_RELA: return "RELA";           // Used for relocation information that modifies code or data addresses during linking.
        case SHT_HASH: return "HASH";           // Used in dynamically linked executables and shared libraries for symbol resolution.
        case SHT_DYNAMIC: return "DYNAMIC";     // Contains information used by the dynamic linker to manage shared libraries.
        case SHT_NOTE: return "NOTE";           // Used for special notes.
        case SHT_NOBITS: return "NOBITS";       // Commonly used for the .bss section, which holds uninitialized global and static variables.
        case SHT_REL: return "REL";             // Similar to SHT_RELA, but the addends are stored in the code or data section itself.
        case SHT_SHLIB: return "SHLIB";         // Not typically used and reserved for future use.
        case SHT_DYNSYM: return "DYNSYM";       // Used in dynamically linked executables and shared libraries for dynamic symbol resolution.
        default: return "UNKNOWN";
    }
}

void interpretELFfile(int fd, state* s, Elf32_Ehdr** header_out, Elf32_Shdr** section_headers_out, const char** strtab_out) {
    if (fd == 1) {
        *header_out = (Elf32_Ehdr*) s->map_start1;
        *section_headers_out = (Elf32_Shdr*) (s->map_start1 + (*header_out)->e_shoff);
        // This line sets *strtab_out to point to the start of the section header string table, allowing access to the names of the sections in the ELF file.
        *strtab_out = (const char*) (s->map_start1 + (*section_headers_out)[(*header_out)->e_shstrndx].sh_offset);
        printf("File: %s\n", s->file_name1);
    } else if (fd == 2) {
        *header_out = (Elf32_Ehdr*) s->map_start2;
        *section_headers_out = (Elf32_Shdr*) (s->map_start2 + (*header_out)->e_shoff);
        *strtab_out = (const char*) (s->map_start2 + (*section_headers_out)[(*header_out)->e_shstrndx].sh_offset);
        printf("File: %s\n", s->file_name2);
    }

}

void print_section_names_helper(int fd, state* s) {
    Elf32_Ehdr* header;
    Elf32_Shdr* section_headers;
    const char* strtab;

    interpretELFfile(fd, s, &header, &section_headers, &strtab);

    for (int i = 0; i < header->e_shnum; i++) {
        printf("[%d] %s %#08x %d %d %s\n", i,
               strtab + section_headers[i].sh_name,
               section_headers[i].sh_addr,
               section_headers[i].sh_offset,
               section_headers[i].sh_size,
               get_section_type(section_headers[i].sh_type));
    }
}

void print_section_names(state* s) {
    if (s->fd1 == -1 && s->fd2 == -1) {
        printf("No ELF files are currently examined\n");
        return;
    }

    if (s->fd1 != -1) {
        print_section_names_helper(1, s);
    }

    if (s->fd2 != -1) {
        print_section_names_helper(2, s);
    }
}


/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// Print symbols functions
void print_symbols_for_file_helper(const Elf32_Ehdr* header, const Elf32_Shdr* section_headers, const char* strtab, const Elf32_Sym* symtab, const char* symtab_strtab, const Elf32_Shdr* symtab_section, state* s, int fd) {
    int num_symbols = symtab_section->sh_size / symtab_section->sh_entsize;

    if (s->debug_mode) {
        printf("Symbol table size: %d\n", symtab_section->sh_size);
        printf("Number of symbols: %d\n", num_symbols);
    }

    printf("File: %s\n", fd == 1 ? s->file_name1 : s->file_name2);
    for (int i = 0; i < num_symbols; i++) {
        const Elf32_Sym* sym = &symtab[i];
        const char* sym_name = symtab_strtab + sym->st_name; // Get the symbol name
        const char* sec_name = (sym->st_shndx < header->e_shnum) ? (strtab + section_headers[sym->st_shndx].sh_name) : "UND"; // Get the section name or "UND" if undefined
        // Print symbol information: index, value, section index, section name, symbol name
        printf("[%d] %08x %d %s %s\n", i, sym->st_value, sym->st_shndx, sec_name, sym_name);
    }
}

void print_symbols_for_file(int fd, state* s) {
    // Referenced from here : https://stackoverflow.com/questions/17029206/searching-init-section-in-elf-file
    Elf32_Ehdr* header;
    Elf32_Shdr* section_headers;
    const char* strtab;

    interpretELFfile(fd, s, &header, &section_headers, &strtab);

    // Find the symbol table section
    Elf32_Shdr* symtab_section = NULL;
    for (int i = 0; i < header->e_shnum; i++) { // The ELF header contains the number of section headers (e_shnum)
        // HT_SYMTAB is a constant that identifies the symbol table section 
        if (section_headers[i].sh_type == SHT_SYMTAB) { 
            // If it is, set symtab_section to point to this section header
            symtab_section = &section_headers[i]; 
            break;
        }
    }

    if (symtab_section == NULL) {
        printf("No symbol table found.\n");
        return;
    }

    void* address_file = ((fd == 1) ? s->map_start1 : s->map_start2);
    Elf32_Sym* symtab = (Elf32_Sym*) (address_file + symtab_section->sh_offset);
    // Calculates the address of the string table used by the symbol table, in order to get the exact memory location of the string table.
    const char* symtab_strtab = (const char*) (address_file + section_headers[symtab_section->sh_link].sh_offset);

    print_symbols_for_file_helper(header, section_headers, strtab, symtab, symtab_strtab, symtab_section, s, fd);
}

void print_symbols(state* s) {
    if (s->fd1 == -1 && s->fd2 == -1) {
        printf("No ELF files are currently examined\n");
        return;
    }

    if (s->fd1 != -1) {
        print_symbols_for_file(1, s);
    }

    if (s->fd2 != -1) {
        print_symbols_for_file(2, s);
    }
}

// void stub_function(state* s) {
//     printf("Not implemented yet\n");
// }

void find_Symbol_Table_Sections(const Elf32_Ehdr* header1, const Elf32_Ehdr* header2, const Elf32_Shdr* section_headers1, const Elf32_Shdr* section_headers2, Elf32_Shdr** symtab_section1, Elf32_Shdr** symtab_section2) {
    *symtab_section1 = NULL;
    *symtab_section2 = NULL;
    for (int i = 0; i < header1->e_shnum; i++) {
        if (section_headers1[i].sh_type == SHT_SYMTAB) {
            *symtab_section1 = (Elf32_Shdr*) &section_headers1[i];
            break;
        }
    }
    for (int i = 0; i < header2->e_shnum; i++) {
        if (section_headers2[i].sh_type == SHT_SYMTAB) {
            *symtab_section2 = (Elf32_Shdr*) &section_headers2[i];
            break;
        }
    }
}

void compareSymbols(int num_symbols1, int num_symbols2, Elf32_Sym* symtab1, Elf32_Sym* symtab2, const char* symtab_strtab1, const char* symtab_strtab2) {
    // Compares symbols from two ELF files and checks for undefined or multiply defined symbols
    for (int i = 1; i < num_symbols1; i++) { // Skip symbol 0, the dummy symbol
        Elf32_Sym* sym = &symtab1[i];
        const char* sym_name = symtab_strtab1 + sym->st_name;
        bool found = false;

        for (int j = 1; j < num_symbols2; j++) {
            Elf32_Sym* sym2 = &symtab2[j];
            const char* sym_name2 = symtab_strtab2 + sym2->st_name;

            if (strcmp(sym_name, sym_name2) == 0) {
                found = true;
                if (sym->st_shndx == SHN_UNDEF && sym2->st_shndx == SHN_UNDEF) {
                    printf("Symbol %s undefined in both files.\n", sym_name);
                } else if (sym->st_shndx != SHN_UNDEF && sym2->st_shndx != SHN_UNDEF) {
                    printf("Symbol %s multiply defined.\n", sym_name);
                }
                break;
            }
        }
        if (!found && sym->st_shndx == SHN_UNDEF) {
            printf("Symbol %s undefined.\n", sym_name);
        }
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// Merge files functions
bool check_merge_helper(state* s, Elf32_Shdr** symtab_section1, Elf32_Shdr** symtab_section2, Elf32_Shdr** section_headers1, Elf32_Shdr** section_headers2, const char** strtab1, const char** strtab2) {
    if (s->fd1 == -1 || s->fd2 == -1) {
        printf("Two ELF files must be opened and mapped.\n");
        return false;
    }

    // Get headers and section headers for both files
    Elf32_Ehdr* header1;
    interpretELFfile(1, s, &header1, section_headers1, strtab1);

    Elf32_Ehdr* header2;
    interpretELFfile(2, s, &header2, section_headers2, strtab2);

    // Find symbol table sections
    find_Symbol_Table_Sections(header1, header2, *section_headers1, *section_headers2, symtab_section1, symtab_section2);

    // Check if symbol tables are found in both files
    if (!*symtab_section1 || !*symtab_section2) {
        printf("Feature not supported: Each file must contain exactly one symbol table.\n");
        return false;
    }

    return true;
}

void check_merge(state* s) {
    Elf32_Shdr* symtab_section1;
    Elf32_Shdr* symtab_section2;
    Elf32_Shdr* section_headers1;
    Elf32_Shdr* section_headers2;
    const char* strtab1;
    const char* strtab2;

    if (!check_merge_helper(s, &symtab_section1, &symtab_section2, &section_headers1, &section_headers2, &strtab1, &strtab2)) {
        return;
    }

    void* address_file1 = s->map_start1;
    Elf32_Sym* symtab1 = (Elf32_Sym*) (address_file1 + symtab_section1->sh_offset);
    const char* symtab_strtab1 = (const char*) (address_file1 + section_headers1[symtab_section1->sh_link].sh_offset);

    void* address_file2 = s->map_start2;
    Elf32_Sym* symtab2 = (Elf32_Sym*) (address_file2 + symtab_section2->sh_offset);
    const char* symtab_strtab2 = (const char*) (address_file2 + section_headers2[symtab_section2->sh_link].sh_offset);

    int num_symbols1 = symtab_section1->sh_size / symtab_section1->sh_entsize;
    int num_symbols2 = symtab_section2->sh_size / symtab_section2->sh_entsize;

    compareSymbols(num_symbols1, num_symbols2, symtab1, symtab2, symtab_strtab1, symtab_strtab2);
}

void sections_Merging(state* s, Elf32_Ehdr* header1, const char* strtab1, Elf32_Shdr* section_headers1, Elf32_Shdr* section_headers2, size_t* offset, Elf32_Shdr* new_section_headers, FILE* out_file, const char* merge_sections[], int merge_sections_length) {
    for (int i = 0; i < header1->e_shnum; i++) {
        const char* section_name = strtab1 + section_headers1[i].sh_name;
        printf("Processing section: %s\n", section_name);

        // These sections do not need to be merged because they are self-contained and do not depend on the content of other sections.
        if (strcmp(section_name, ".shstrtab") == 0 || strcmp(section_name, ".symtab") == 0 || strcmp(section_name, ".strtab") == 0) {
            // Copy these sections directly without merging
            fwrite((char*)s->map_start1 + section_headers1[i].sh_offset, 1, section_headers1[i].sh_size, out_file);  // Writes the section data from the first ELF file to the output file.
            new_section_headers[i].sh_offset = *offset;
            *offset += section_headers1[i].sh_size;    // Updates the offset for the next section.
            printf("Copying section: %s\n", section_name);
            printf("Updated offset: %zu\n", *offset);
        } else {
            // Merge sections
            for (int j = 0; j < merge_sections_length; j++) {
                if (strcmp(section_name, merge_sections[j]) == 0) {
                    printf("Merging section: %s\n", section_name);
                    fwrite((char*)s->map_start1 + section_headers1[i].sh_offset, 1, section_headers1[i].sh_size, out_file);
                    fwrite((char*)s->map_start2 + section_headers2[i].sh_offset, 1, section_headers2[i].sh_size, out_file);
                    new_section_headers[i].sh_size = section_headers1[i].sh_size + section_headers2[i].sh_size;
                    new_section_headers[i].sh_offset = *offset;
                    *offset += new_section_headers[i].sh_size;
                    printf("Merged section %s: new size %u, new offset %zu\n", section_name, new_section_headers[i].sh_size, *offset);
                    break;
                }
            }
        }
    }
    printf("Finished sections_Merging\n");
}

void merge_elf_files(state* s) {
    printf("Starting merge_elf_files\n");
    Elf32_Shdr* symtab_section1;
    Elf32_Shdr* symtab_section2;
    Elf32_Shdr* section_headers1;
    Elf32_Shdr* section_headers2;
    const char* strtab1 = NULL;
    const char* strtab2 = NULL;

    // Open the output file
    FILE* out_file = fopen("out.ro", "wb");
    if (!out_file) {
        perror("Error opening output file");
        return;
    }

    if (!check_merge_helper(s, &symtab_section1, &symtab_section2, &section_headers1, &section_headers2, &strtab1, &strtab2)) {
        fclose(out_file);  // Close the output file if the merge check fails
        return;
    }

    Elf32_Ehdr* header1 = (Elf32_Ehdr*) s->map_start1;

    fwrite(header1, 1, sizeof(Elf32_Ehdr), out_file);

    Elf32_Shdr* new_section_headers = malloc(header1->e_shnum * sizeof(Elf32_Shdr));      // Allocates memory for the new section header table specified by header1->e_shnum.
    if (new_section_headers == NULL) {
        perror("Error allocating memory for new section headers");
        fclose(out_file);
        return;
    }
    memcpy(new_section_headers, section_headers1, header1->e_shnum * sizeof(Elf32_Shdr)); // Copies the section header table

    // Merge sections .text, .data, .rodata
    const char* merge_sections[] = {".text", ".data", ".rodata"};
    size_t offset = sizeof(Elf32_Ehdr); // Start offset after the ELF header

    sections_Merging(s, header1, strtab1, section_headers1, section_headers2, &offset, new_section_headers, out_file, merge_sections, sizeof(merge_sections) / sizeof(merge_sections[0]));
    
    // Write the new section header table at the end of the file
    Elf32_Off shoff = ftell(out_file);  // Gets the current file position, which will be the offset for the new section header table.
    
    fwrite(new_section_headers, 1, header1->e_shnum * sizeof(Elf32_Shdr), out_file);
    
    // Update the ELF header with the new section header table offset
    header1->e_shoff = shoff;
    fseek(out_file, 0, SEEK_SET);  // Repositions the file pointer to the beginning of the file.
    fwrite(header1, 1, sizeof(Elf32_Ehdr), out_file);  // Writes the updated ELF header to the file.
    

    printf("Writing new section headers\n");
    for (int i = 0; i < header1->e_shnum; i++) {
        printf("Section %d: offset %u, size %u\n", i, new_section_headers[i].sh_offset, new_section_headers[i].sh_size);
    }

    // Cleanup
    free(new_section_headers);
    fclose(out_file);

    printf("Merged ELF file created: out.ro\n");
}


/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

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
    print_section_names,
    print_symbols,
    check_merge,
    merge_elf_files, 
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

/*void sections_Merging(state* s, Elf32_Ehdr* header1, const char* strtab1, Elf32_Shdr* section_headers1, Elf32_Shdr* section_headers2, size_t* offset, Elf32_Shdr* new_section_headers, FILE* out_file, const char* merge_sections[], int merge_sections_length) {
    for (int i = 0; i < header1->e_shnum; i++) {
        const char* section_name = strtab1 + section_headers1[i].sh_name;

        // These sections do not need to be merged because they are self-contained and do not depend on the content of other sections.
        if (strcmp(section_name, ".shstrtab") == 0 || strcmp(section_name, ".symtab") == 0 || strcmp(section_name, ".strtab") == 0) {
            // Copy these sections directly without merging
            fwrite((char*)s->map_start1 + section_headers1[i].sh_offset, 1, section_headers1[i].sh_size, out_file);  // Writes the section data from the first ELF file to the output file.
            new_section_headers[i].sh_offset = *offset;
            *offset += section_headers1[i].sh_size;    // Updates the offset for the next section.
        } else {
            // Merge sections
            for (int j = 0; j < merge_sections_length; j++) {
                if (strcmp(section_name, merge_sections[j]) == 0) {
                    fwrite((char*)s->map_start1 + section_headers1[i].sh_offset, 1, section_headers1[i].sh_size, out_file);
                    fwrite((char*)s->map_start2 + section_headers2[i].sh_offset, 1, section_headers2[i].sh_size, out_file);
                    new_section_headers[i].sh_size = section_headers1[i].sh_size + section_headers2[i].sh_size;
                    new_section_headers[i].sh_offset = *offset;
                    *offset += new_section_headers[i].sh_size;
                    break;
                }
            }
        }
    }
}

void merge_elf_files(state* s) {
    Elf32_Shdr* symtab_section1;
    Elf32_Shdr* symtab_section2;
    Elf32_Shdr* section_headers1;
    Elf32_Shdr* section_headers2;
    const char* strtab1 = NULL;
    const char* strtab2 = NULL;

    // Open the output file
    FILE* out_file = fopen("out.ro", "wb");
    if (!out_file) {
        perror("Error opening output file");
        return;
    }

    if (!check_merge_helper(s, &symtab_section1, &symtab_section2, &section_headers1, &section_headers2, &strtab1, &strtab2)) {
        fclose(out_file);  // Close the output file if the merge check fails
        return;
    }

    Elf32_Ehdr* header1 = (Elf32_Ehdr*) s->map_start1;

    fwrite(header1, 1, sizeof(Elf32_Ehdr), out_file);

    Elf32_Shdr* new_section_headers = malloc(header1->e_shnum * sizeof(Elf32_Shdr));      // Allocates memory for the new section header table specified by header1->e_shnum.
    memcpy(new_section_headers, section_headers1, header1->e_shnum * sizeof(Elf32_Shdr)); // Copies the section header table

    // Merge sections .text, .data, .rodata
    const char* merge_sections[] = {".text", ".data", ".rodata"};
    size_t offset = sizeof(Elf32_Ehdr); // Start offset after the ELF header

    sections_Merging(s, header1, strtab1, section_headers1, section_headers2, &offset, new_section_headers, out_file, merge_sections, sizeof(merge_sections) / sizeof(merge_sections[0]));

    // Write the new section header table at the end of the file
    long shoff = ftell(out_file);  // Gets the current file position, which will be the offset for the new section header table.
    fwrite(new_section_headers, 1, header1->e_shnum * sizeof(Elf32_Shdr), out_file);

    // Update the ELF header with the new section header table offset
    header1->e_shoff = shoff;
    fseek(out_file, 0, SEEK_SET);  // Repositions the file pointer to the beginning of the file.
    fwrite(header1, 1, sizeof(Elf32_Ehdr), out_file);  // Writes the updated ELF header to the file.

    // Cleanup
    free(new_section_headers);
    fclose(out_file);

    printf("Merged ELF file created: out.ro\n");
}
*/
