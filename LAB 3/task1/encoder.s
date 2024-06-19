section .data
    char db 0              ; Storage for a character
    inFile dd 0            ; File descriptor for input file
    outFile dd 1           ; File descriptor for output file (stdout)
    newline db 0xA         ; Newline character

section .text
global main

; File descriptors numbers
%define STDIN 0
%define STDOUT 1
%define STDERR 2
; Syscall numbers
%define SYS_READ 0x3
%define SYS_WRITE 0x4
%define SYS_OPEN 0x5
%define SYS_CLOSE 0x6
%define SYS_EXIT 0x1

; File open flags
%define O_RDONLY 0x0
%define O_WRONLY 0x1
%define O_CREAT 0x40
%define O_TRUNC 0x200

extern strlen

main:
    ; Parsing command-line arguments
    ; edi = argc, esi = argv
    mov edi, [esp+4]       ; Get argc
    mov esi, [esp+8]       ; Get argv

    loop_argv:
        mov ecx, [esi]     ; Get current argv[i]
    get_argument_length:
        push ecx
        call strlen        ; eax = strlen(ecx) = strlen(argv[i])
        pop ecx
    print_argument:
        mov edx, eax       ; edx = length of argv[i] - Move length of argv[i] to edx.
        mov eax, SYS_WRITE ; Prepare to write to stderr
        mov ebx, STDERR
        int 0x80           ; Write argument to stderr
        mov eax, SYS_WRITE
        mov ebx, STDERR
        mov ecx, newline
        mov edx, 1         ; Set length to 1
        int 0x80           ; Write newline to stderr
    scan_argument:
        mov ecx, [esi]     ; ecx = current argument
        cmp byte [ecx], '-'
        je check_i_or_o
    continue_loop:
        sub edi, 1         ; Decrement argc
        add esi, 4         ; Move to the next argv[i]
        cmp edi, 0         ; If all arguments processed, end loop
        jne loop_argv
        jmp encoder        ; Start encoding if no more arguments

check_i_or_o:
    cmp word [ecx+1], 'i'
    je open_input_file     ; If "-i", jump to open_input_file
    cmp word [ecx+1], 'o'
    je open_output_file    ; If "-o", jump to open_output_file
    jmp continue_loop      ; Continue to next argument if not "-i" or "-o"

encoder:
    read_char:
        mov eax, SYS_READ
        mov ebx, [inFile]  ; Set file descriptor for input
        mov ecx, char
        mov edx, 1         ; Set number of bytes to read
        int 0x80           ; Read a character
    check_eof:
        cmp eax, 0
        jle exit_program   ; If end of file, exit program
    should_encode:
        cmp byte [char], 'A'
        jl print_char      ; If char < 'A', print as is
        cmp byte [char], 'z'
        jg print_char      ; If char > 'z', print as is
        add byte [char], 1 ; Increment character for encoding
    print_char:
        mov eax, SYS_WRITE
        mov ebx, [outFile]
        mov ecx, char
        mov edx, 1
        int 0x80           ; Write the encoded/unchanged character
        jmp encoder        ; Continue reading next character

exit_program:
    mov eax, SYS_EXIT
    mov ebx, 0
    int 0x80               ; Exit program

open_input_file:
    mov eax, SYS_OPEN
    add ecx, 2
    mov ebx, ecx           ; Set file name for input
    mov ecx, O_RDONLY      ; Open file in read-only mode
    int 0x80
    cmp eax, 0             ; Check if the file was opened successfully
    jle exit_program       ; Exit if file couldn't be opened
    mov [inFile], eax      ; Save file descriptor for input
    jmp continue_loop      ; Continue to next argument

open_output_file:
    mov eax, SYS_OPEN
    add ecx, 2
    mov ebx, ecx           ; Set file name for output
    mov ecx, O_WRONLY      ; Open file in write-only mode
    or ecx, O_CREAT        ; Create file if it doesn't exist
    or ecx, O_TRUNC        ; Truncate file to zero length if it exists
    mov edx, 0644o         ; Set file permissions
    int 0x80
    cmp eax, 0
    jle exit_program       ; Exit if file couldn't be opened
    mov [outFile], eax     ; Save file descriptor for output
    jmp continue_loop      ; Continue to next argument
