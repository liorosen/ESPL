.section .data
    fmt: .asciz "%d\n"
    fmt_hex: .asciz "%02hhx"
    new_line: .asciz "\n"
    arg_fmt: .asciz "Argument %d: %s\n"

.section .bss
    buffer: .space 600

.section .text
    .extern printf, puts, fgets, malloc, free
    .global main, print_multi, get_multi, add_multi, rand_num, PRmulti, MaxMin

main:
    # Save arguments
    movl 4(%esp), %esi         # argc
    movl 8(%esp), %edi         # argv

    # Print argc
    pushl %esi
    pushl $fmt
    call printf
    addl $8, %esp              # Cleans up the stack by adjusting the stack pointer

    # Print argv[i]
    movl %esi, %ecx
    decl %ecx                  # Decrements ecx by 1 (to use it as a counter starting from 0).
    incl %edi                  # Moves edi to point to the first argument in argv.

print_loop:
    testl %ecx, %ecx
    jz print_done              # Jumps to print_done if ecx is zero.
    pushl (%edi)               # Pushes the current argument onto the stack.
    call puts
    addl $4, %esp              # Cleans up the stack
    addl $4, %edi
    decl %ecx
    jmp print_loop             # Jumps back to the beginning of the loop.

print_done:
    ret

print_multi:
    # Input: %esi - pointer to struct multi
    # struct multi { unsigned char size; unsigned char num[] }
    movl (%esi), %ecx          # Load size
    addl $4, %esi              # Moves the pointer to the start of the num array.

print_hex_loop:
    testl %ecx, %ecx
    jz print_hex_done
    movzbl (%esi), %eax
    pushl %eax
    pushl $fmt_hex
    call printf
    addl $8, %esp
    incl %esi
    decl %ecx
    jmp print_hex_loop

print_hex_done:
    pushl $new_line
    call puts
    addl $4, %esp
    ret

get_multi:
    # Input: %esi - pointer to struct multi
    pushl $buffer
    pushl $600
    call fgets
    addl $8, %esp

    # Determine length of input
    movl $buffer, %eax          # Moves the address of the buffer to eax
    xorl %ecx, %ecx
get_multi_length:
    cmpb $0, (%eax)
    je get_multi_done
    incl %ecx
    incl %eax
    jmp get_multi_length

get_multi_done:
    movb %cl, (%esi)            # Set size
    incl %esi                   # Point to num array

    # Convert hex to bytes
    movl $buffer, %eax
    movl %esi, %edx

get_multi_convert:
    testl %ecx, %ecx
    jz get_multi_finish
    movb (%eax), %bl
    call hex_to_byte
    movb %al, (%edx)
    incl %edx
    addl $2, %eax
    subl $2, %ecx
    jmp get_multi_convert

get_multi_finish:
    ret

hex_to_byte:
    # Input: %bl - hex character
    # Output: %al - byte value
    movb %bl, %al
    subb $'0', %al
    cmpb $9, %al
    jle hex_to_byte_done
    subb $7, %al                # Subtracts 7 from al to handle hex characters 'A' to 'F'
    andb $0x0F, %al

hex_to_byte_done:
    ret

add_multi:
    # Input: %esi - pointer to struct multi p, %edi - pointer to struct multi q
    # Output: %eax - pointer to result struct multi
    call MaxMin
    movl %eax, %ebx             # p
    movl %edi, %ecx             # q

    # Allocate memory for result
    movl (%ebx), %edx
    incl %edx
    pushl %edx
    call malloc
    addl $4, %esp

    # Initialize result struct
    movl %edx, (%eax)
    addl $4, %eax
    movl %eax, %edi

    # Perform addition
    xorl %ebx, %ebx             # Clears the ebx register (used as an index).
    xorl %edx, %edx             # Clears the edx register (used as a carry flag).

add_multi_loop:
    testl %edx, %edx
    jz add_multi_done
    movb (%esi,%ebx), %al       # Loads the current byte from the larger number
    addb (%ecx,%ebx), %al
    adcb $0, %al
    movb %al, (%edi,%ebx)
    incl %ebx
    decl %edx
    jmp add_multi_loop

add_multi_done:
    ret

MaxMin:
    # Input: %esi - pointer to struct multi p, %edi - pointer to struct multi q
    # Output: %eax - pointer to struct with max length, %ebx - pointer to struct with min length
    movl (%esi), %eax
    movl (%edi), %ebx
    cmpl %ebx, %eax
    jge maxmin_p
    xchgl %esi, %edi
maxmin_p:
    movl %esi, %eax
    movl %edi, %ebx
    ret

rand_num:
    # Implement a pseudo-random number generator using LFSR
    ret

PRmulti:
    # Implement PRmulti function using rand_num
    ret
