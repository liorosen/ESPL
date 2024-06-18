section .data
message db 'Hello, Infected File', 0xA  ; Message to print
msg_len equ $ - message                 ; Length of the message

section .bss
filename resb 100   ; Reserve space for filename (adjust size as needed)

section .text
global _start
global system_call
global infection
global infector
extern main
extern strlen

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop

system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

infection:
    mov eax, 4              ; sys_write
    mov ebx, 1              ; stdout
    lea ecx, [message]
    mov edx, msg_len
    int 0x80
    ret

infector:
    ; Print the file name
    mov eax, 4              ; sys_write
    mov ebx, 1              ; stdout
    mov ecx, [esp + 4]
    push ecx
    call strlen
    pop ecx
    mov edx, eax            ; length of the filename
    int 0x80

    ; Open the file with append mode
    mov eax, 5              ; sys_open
    pop ebx                 ; filename
    mov ecx, 0x102          ; flags: O_WRONLY | O_APPEND
    mov edx, 0              ; mode (not used)
    int 0x80
    test eax, eax
    js error_exit
    mov edi, eax            ; save file descriptor

    ; Write the virus code to the file
    lea ecx, [code_start]
    mov edx, code_end - code_start
write_loop:
    cmp edx, 0
    je close_file
    mov eax, 4              ; sys_write
    mov ebx, edi            ; file descriptor
    int 0x80
    add ecx, eax
    sub edx, eax
    jmp write_loop

close_file:
    ; Close the file
    mov eax, 6              ; sys_close
    mov ebx, edi
    int 0x80
    ret

code_start:
    ; Place any code here that you want to attach
    ; For example, the message "Hello, Infected File"
    db 'This is the virus code.', 0xA
code_end:

error_exit:
    ; Exit with error code
    mov eax, 1              ; sys_exit
    mov ebx, 0x55           ; exit code
    int 0x80
    ret
