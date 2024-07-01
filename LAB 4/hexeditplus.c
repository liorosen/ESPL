#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    int display_mode; // 0 for decimal, 1 for hexadecimal
} state;

void toggle_debug_mode(state* s) {
    s->debug_mode = !s->debug_mode;
    printf("Debug flag now %s\n", s->debug_mode ? "on" : "off");
}

void set_file_name(state* s) {
    printf("Enter file name: ");
    fgets(s->file_name, sizeof(s->file_name), stdin);
    s->file_name[strcspn(s->file_name, "\n")] = '\0'; // Remove trailing newline
    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void set_unit_size(state* s) {
    int size;
    printf("Enter unit size (1, 2, or 4): ");
    scanf("%d", &size);
    if (size == 1 || size == 2 || size == 4) {
        s->unit_size = size;
        if (s->debug_mode) {
            fprintf(stderr, "Debug: set size to %d\n", s->unit_size);
        }
    } else {
        printf("Invalid unit size\n");
    }
}

//Here i load the file into the memory according to its location and lenght
void load_into_memory(state* s) {
    if (strlen(s->file_name) == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    FILE *file = fopen(s->file_name, "rb");
    if (!file) {
        printf("Error: could not open file %s\n", s->file_name);
        return;
    }

    char input[256];
    unsigned int location;
    int length;

    printf("Please enter <location> <length>: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %d", &location, &length);

    if (s->debug_mode) {
        fprintf(stderr, "Debug: file_name=%s location=%x length=%d\n", s->file_name, location, length);
    }

    fseek(file, location, SEEK_SET);
    size_t bytes_read = fread(s->mem_buf, s->unit_size, length, file);
    s->mem_count = bytes_read * s->unit_size;

    fclose(file);

    printf("Loaded %zu units into memory\n", bytes_read);
}

void toggle_display_mode(state* s) {
    s->display_mode = !s->display_mode;
    printf("Display flag now %s, %s representation\n",
           s->display_mode ? "on" : "off",
           s->display_mode ? "hexadecimal" : "decimal");
}

void memory_display(state* s) {
    char input[256];
    unsigned int addr;
    int length;

    printf("Enter address and length: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %d", &addr, &length);

    unsigned char *start = addr == 0 ? s->mem_buf : (s->mem_buf + addr);

    if (s->display_mode) {
        printf("Hexadecimal\n===========\n");
        for (int i = 0; i < length; i++) {
            switch (s->unit_size) {
                case 1:
                    printf("%#hhx\n", start[i]);
                    break;
                case 2:
                    printf("%#hx\n", ((unsigned short*)start)[i]);
                    break;
                case 4:
                    printf("%#x\n", ((unsigned int*)start)[i]);
                    break;
                default:
                    printf("Invalid unit size\n");
                    return;
            }
        }
    } else {
        printf("Decimal\n=======\n");
        for (int i = 0; i < length; i++) {
            switch (s->unit_size) {
                case 1:
                    printf("%hhd\n", start[i]);
                    break;
                case 2:
                    printf("%hd\n", ((unsigned short*)start)[i]);
                    break;
                case 4:
                    printf("%d\n", ((unsigned int*)start)[i]);
                    break;
                default:
                    printf("Invalid unit size\n");
                    return;
            }
        }
    }
}

void save_into_file(state* s) {
    if (strlen(s->file_name) == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    FILE *file = fopen(s->file_name, "r+b");
    if (!file) {
        printf("Error: could not open file %s\n", s->file_name);
        return;
    }

    char input[256];
    unsigned int source_addr;
    unsigned int target_location;
    int length;

    printf("Please enter <source-address> <target-location> <length>: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x %d", &source_addr, &target_location, &length);

    if (source_addr + length * s->unit_size > sizeof(s->mem_buf)) {
        printf("Error: source address out of bounds\n");
        fclose(file);
        return;
    }

    unsigned char *source = source_addr == 0 ? s->mem_buf : (s->mem_buf + source_addr);

    if (s->debug_mode) {
        fprintf(stderr, "Debug: source-address=%x target-location=%x length=%d\n", source_addr, target_location, length);
    }

    fseek(file, target_location, SEEK_SET);
    size_t bytes_written = fwrite(source, s->unit_size, length, file);

    fclose(file);

    if (bytes_written == length) {
        printf("Saved %zu units into file\n", bytes_written);
    } else {
        printf("Error: only %zu units were written\n", bytes_written);
    }
}

void memory_modify(state* s) {
    char input[256];
    unsigned int location;
    unsigned int val;

    printf("Please enter <location> <val>: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x", &location, &val);

    if (location + s->unit_size > sizeof(s->mem_buf)) {
        printf("Error: location out of bounds\n");
        return;
    }

    if (s->debug_mode) {
        fprintf(stderr, "Debug: location=%x val=%x\n", location, val);
    }

    unsigned char *target = s->mem_buf + location;

    switch (s->unit_size) {
        case 1:
            *target = (unsigned char)val;
            break;
        case 2:
            *(unsigned short*)target = (unsigned short)val;
            break;
        case 4:
            *(unsigned int*)target = (unsigned int)val;
            break;
        default:
            printf("Invalid unit size\n");
            return;
    }

    printf("Modified memory at location %x with value %x\n", location, val);
}

void quit(state* s) {
    if (s->debug_mode) {
        fprintf(stderr, "quitting\n");
    }
    exit(0);
}

void not_implemented(state* s) {
    printf("Not implemented yet\n");
}

typedef void (*menu_function)(state* s);

menu_function menu_functions[] = {
    toggle_debug_mode,
    set_file_name,
    set_unit_size,
    load_into_memory,
    toggle_display_mode,
    memory_display,
    save_into_file,
    memory_modify, // Memory Modify
    quit,
    NULL
};

const char* menu[] = {
    "0-Toggle Debug Mode",
    "1-Set File Name",
    "2-Set Unit Size",
    "3-Load Into Memory",
    "4-Toggle Display Mode",
    "5-Memory Display",
    "6-Save Into File",
    "7-Memory Modify",
    "8-Quit",
    NULL
};

void print_debug_info(state* s) {
    if (s->debug_mode) {
        fprintf(stderr, "Debug: unit_size=%d file_name=%s mem_count=%zu display_mode=%s\n",
                s->unit_size, s->file_name, s->mem_count,
                s->display_mode ? "hexadecimal" : "decimal");
    }
}

void print_menu() {
    for (int i = 0; menu[i] != NULL; i++) {
        printf("%s\n", menu[i]);
    }
}

int main() {
    state s = {0, "", 1, {0}, 0, 0}; // Initialize state
    int choice;

    while (1) {
        print_debug_info(&s);
        print_menu();
        printf("> ");
        scanf("%d", &choice);
        getchar(); // Consume the newline character after scanf
        if (choice >= 0 && choice <= 8) {
            menu_functions[choice](&s);
        } else {
            printf("Invalid choice\n");
        }
    }
    return 0;
}
