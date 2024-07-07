section .data
    fmt db "%02hhx", 0        ; Format string for printing hexadecimal
    nl db 10                   ; Newline character

section .text
    global print_multi
    extern printf, puts

print_multi:
    ; Debugging output to trace execution
    pushad                     ; Save all registers
    push dword eax             ; Print address of struct multi
    push dword fmt
    call printf
    add esp, 8
    popad                      ; Restore all registers

    ; Load parameters passed from main
    mov eax, [esp + 4]         ; Load pointer to struct multi from stack

    ; Extract size and num array pointer from struct multi
    mov cl, [eax]              ; Load size of num array (ecx = cl)
    lea ebx, [eax + 1]         ; Pointer to num array (ebx = eax + 1)

    ; Print num array in little endian hexadecimal
    xor edx, edx               ; Clear edx for index (edx = 0)

print_loop:
    cmp edx, ecx               ; Compare index with size
    jge end_print              ; Exit loop if index >= size

    ; Debugging output to trace loop iteration
    pushad
    push edx                   ; Print current index
    push dword fmt
    call printf
    add esp, 8
    popad

    ; Print num[edx] using printf("%02hhx")
    movzx eax, byte [ebx + edx]   ; Load num[edx] into eax
    push eax                      ; Push num[edx]
    push dword fmt                ; Push format string "%02hhx"
    call printf                   ; Call printf(num[edx], "%02hhx")
    add esp, 8                    ; Clean up stack

    inc edx                  ; Increment index
    jmp print_loop           ; Repeat for next byte

end_print:
    ; Debugging output to trace end of function
    pushad
    push dword nl             ; Print newline character
    call puts
    add esp, 4
    popad

    ; Exit the function
    ret
