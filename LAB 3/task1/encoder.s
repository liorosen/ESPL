section .data
    char db 0              ; Storage for a character
    newline db 0xA         ; Newline character

section .bss
    inFile resd 1          ; File descriptor for input file
    outFile resd 1         ; File descriptor for output file

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

    ; Initialize default file descriptors
    mov dword [inFile], STDIN
    mov dword [outFile], STDOUT

    ; Process command-line arguments
    
loop_argv:
    sub edi, 1
    js start_encoding      ; If no more arguments, start encoding
    mov ecx, [esi]         ; Get current argv[i]
    add esi, 4             ; Move to the next argument

    cmp byte [ecx], '-'    ; Check if argument starts with '-'
    jne loop_argv          ; If not, continue to the next argument

    cmp byte [ecx+1], 'i'
    je open_input_file
    cmp byte [ecx+1], 'o'
    je open_output_file
    jmp loop_argv          ; Continue processing arguments

open_input_file:
    add ecx, 2             ; Skip "-i"
    mov eax, SYS_OPEN      ; sys_open
    mov ebx, ecx           ; File name
    mov ecx, O_RDONLY      ; Open read-only
    int 0x80
    test eax, eax
    js error_exit          ; If open failed, exit
    mov [inFile], eax      ; Save file descriptor for input
    jmp loop_argv

open_output_file:
    add ecx, 2             ; Skip "-o"
    mov eax, SYS_OPEN      ; sys_open
    mov ebx, ecx           ; File name
    mov ecx, O_WRONLY      ; Open write-only
    or ecx, O_CREAT        ; Create if it doesn't exist
    or ecx, O_TRUNC        ; Truncate file to zero length if it exists
    mov edx, 0644o         ; File permissions
    int 0x80
    test eax, eax
    js error_exit          ; If open failed, exit
    mov [outFile], eax     ; Save file descriptor for output
    jmp loop_argv

start_encoding:
    ; Start encoding process
    pushad                 ; Save all general-purpose registers

read_char:
    mov eax, SYS_READ      ; sys_read
    mov ebx, [inFile]      ; stdin or input file
    lea ecx, [char]        ; Buffer to store character
    mov edx, 1             ; Read one byte
    int 0x80
    test eax, eax
    jle close_files        ; If end of file, close files

    ; Encoding the character
    cmp byte [char], 'A'
    jl write_char
    cmp byte [char], 'z'
    jg write_char
    add byte [char], 1     ; Increment character for encoding

write_char:
    mov eax, SYS_WRITE     ; sys_write
    mov ebx, [outFile]     ; stdout or output file
    lea ecx, [char]        ; Character to write
    mov edx, 1             ; Write one byte
    int 0x80
    jmp read_char

close_files:
    mov eax, SYS_CLOSE
    mov ebx, [inFile]
    int 0x80               ; Close input file
    mov eax, SYS_CLOSE
    mov ebx, [outFile]
    int 0x80               ; Close output file
    jmp exit_program

error_exit:
    mov eax, SYS_EXIT
    mov ebx, 0x55          ; Exit with error code
    int 0x80

exit_program:
    mov eax, SYS_EXIT
    mov ebx, 0             ; Exit with success code
    int 0x80
