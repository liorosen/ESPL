%define SYS_READ 0x3
%define SYS_WRITE 0x4
%define SYS_OPEN 0x5
%define SYS_CLOSE 0x6
%define SYS_EXIT 0x1

%define O_RDONLY 0x0
%define O_WRONLY 0x1
%define O_CREAT 0x40
%define O_TRUNC 0x200

%define STDIN 0
%define STDOUT 1

section .bss
inFile resd 1
outFile resd 1
char resb 1

section .data
newline db 0xA

section .text
global main
extern strlen
; reference in this ebsite : https://www.tutorialspoint.com/assembly_programming/assembly_loops.htm
main:
    push ebp
    mov ebp, esp            ; setting up the stack frame
    mov eax, [ebp+8]        ; argc
    mov ebx, [ebp+12]       ; argv

    mov dword [inFile], STDIN  ; Initialize inFile to 0 (stdin)
    mov dword [outFile], STDOUT ; Initialize outFile to 1 (stdout)

    sub eax, 1              ; Decrement argc in order to skip the program name
    jz encode               ; if(argc == 0), jump to encode- start encoding
    add ebx, 4              ; Move to the next argument(argv[1])

scan_arguments:
    mov ecx, [ebx]          ; Load current argument
    cmp byte [ecx], '-'     ; Check if argument starts with '-'
    jne next_arg
    cmp byte [ecx+1], 'i'
    je open_input_file
    cmp byte [ecx+1], 'o'
    je open_output_file
next_arg:
    sub eax, 1              ; Decrement argc
    jz encode               ; If no more arguments, start encoding
    add ebx, 4              ; Move to the next argument
    jmp scan_arguments      ; Loop to scan_arguments

open_input_file:
    call skip_and_open
    mov ecx, O_RDONLY       ; O_RDONLY
    int 0x80                ; Open the file
    test eax, eax
    js error_exit
    mov dword [inFile], eax ; Save the file descriptor to inFile
    jmp next_arg

open_output_file:
    call skip_and_open
    mov ecx, O_WRONLY       ; O_WRONLY
    or ecx, O_CREAT         ; O_CREAT
    or ecx, O_TRUNC         ; O_TRUNC
    mov edx, 0644o          ; File permissions
    int 0x80                ; Open the file
    test eax, eax
    js error_exit
    mov dword [outFile], eax ; Save the file descriptor to outFile
    jmp next_arg

encode:
    pushad                  ; Save all general-purpose registers
read_char:
    mov eax, SYS_READ       ; sys_read
    mov ebx, [inFile]       ; stdin or input file
    lea ecx, [char]         ; Buffer to store character
    mov edx, 1              ; Read one byte
    int 0x80
    cmp eax, 0              ; Check if end of file
    jle done            
    ; Encoding the character part:
    cmp byte [char], 'A'
    jl print_char
    cmp byte [char], 'z'
    jg print_char
    add byte [char], 1
print_char:
    mov eax, SYS_WRITE      ; sys_write
    mov ebx, [outFile]      ; stdout or output file
    lea ecx, [char]         ; Character to write
    mov edx, 1              ; Write one byte
    int 0x80
    jmp read_char

done:
    popad                   ; Restore all general-purpose registers
    mov eax, SYS_EXIT       ; sys_exit
    mov ebx, 0              ; exit code
    int 0x80                ; Exit

skip_and_open:
    add ecx, 2              ; Skip "-o" or "-i"
    mov eax, SYS_OPEN       ; sys_open
    mov ebx, ecx            ; File name
    ret

error_exit:
    mov eax, SYS_EXIT       ; sys_exit
    mov ebx, 0x55           ; exit code
    int 0x80                ; Make the system call to exit
    ret
