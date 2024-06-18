line: db "argv", 10, 0
newline: db 10, 0

section .text
global _start
global system_call
extern strlen



_start:
    ; Save argc (argument count) and argv (argument vector)
    pop ecx                 ; Store argc in ecx
    mov esi, esp            ; Store argv in esi

    ; Calculate the address of envp (environment pointer)
    mov eax, ecx            ; Copy argc to eax
    shl eax, 2              ; Multiply argc by 4 (size of each pointer)
    add eax, esi            ; Add the size to the address of argv
    add eax, 4              ; Skip NULL at the end of argv
    push eax                ; Push envp onto the stack
    push esi                ; Push argv onto the stack
    push ecx                ; Push argc onto the stack

    ; Call the main function
    call main               ; Call main(argc, argv, envp)

    

    ; Exit the program
    mov ebx, eax            ; Store the return value of main in ebx
    mov eax, 1              ; System call number (sys_exit)
    int 0x80                ; Invoke the system call

system_call:
    push ebp                ; Save the base pointer
    mov ebp, esp            ; Set the base pointer to the current stack pointer
    sub esp, 4              ; Allocate space for a local variable
    pushad                  ; Save general-purpose registers

    ; Load system call arguments into registers
    mov eax, [ebp+8]        ; First argument
    mov ebx, [ebp+12]       ; Second argument
    mov ecx, [ebp+16]       ; Third argument
    mov edx, [ebp+20]       ; Fourth argument
    int 0x80                ; Make the system call

    mov [ebp-4], eax        ; Save the return value
    popad                   ; Restore general-purpose registers
    mov eax, [ebp-4]        ; Move the return value to eax
    add esp, 4              ; Deallocate local variable space
    pop ebp                 ; Restore the base pointer
    ret                     ; Return to the caller




main:

    

    push ebp
    mov ebp, esp
    mov eax,[ebp+8]  ;argc
    mov ebx,[ebp+12] ;argv

    
loop:
    pushad

    push dword [ebx] 
    call strlen
    add esp,4
    mov edx,eax
    
    mov eax, 4
    mov ecx,[ebx]
    ;mov ecx, [ecx]
    mov ebx,1  
    int 0x80

; newline 
    mov eax, 4
    mov ebx,1
    mov ecx,newline
    mov edx,1
    int 0x80

    popad

    add ebx, 4
    sub eax,1
    cmp eax,0
    jne loop

    pop ebp
    ret
