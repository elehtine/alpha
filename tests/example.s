.global main
.type main, @function
.global print_int
.global print_bool
.global read_int

.section .text

print_int:
    pushq %rbp               # Save previous stack frame pointer
    movq %rsp, %rbp          # Set stack frame pointer
    movq %rdi, %r10          # Back up original input
    decq %rsp                # Point rsp at first byte of output
                             # TODO: this non-alignment confuses debuggers. Use a different register?

    # Add newline as the last output byte
    movb $10, (%rsp)         # ASCII newline = 10
    decq %rsp

    # Check for zero and negative cases
    xorq %r9, %r9
    xorq %rax, %rax
    cmpq $0, %rdi
    je .Ljust_zero
    jge .Ldigit_loop
    incq %r9  # If < 0, set %r9 to 1

.Ldigit_loop:
    cmpq $0, %rdi
    je .Ldigits_done        # Loop done when input = 0

    # Divide rdi by 10
    movq %rdi, %rax
    movq $10, %rcx
    cqto
    idivq %rcx               # Sets rax = quotient and rdx = remainder

    movq %rax, %rdi          # The quotient becomes our remaining input
    cmpq $0, %rdx            # If the remainder is negative (because the input is), negate it
    jge .Lnot_negative
    negq %rdx
.Lnot_negative:
    addq $48, %rdx           # ASCII '0' = 48. Add the remainder to get the correct digit.
    movb %dl, (%rsp)         # Store the digit in the output
    decq %rsp
    jmp .Ldigit_loop

.Ljust_zero:
    movb $48, (%rsp)         # ASCII '0' = 48
    decq %rsp

.Ldigits_done:

    # Add minus sign if negative
    cmpq $0, %r9
    je .Lminus_done
    movb $45, (%rsp)         # ASCII '-' = 45
    decq %rsp
.Lminus_done:

    # Call syscall 'write'
    movq $1, %rax            # rax = syscall number for write
    movq $1, %rdi            # rdi = file handle for stdout
    # rsi = pointer to message
    movq %rsp, %rsi
    incq %rsi
    # rdx = number of bytes
    movq %rbp, %rdx
    subq %rsp, %rdx
    decq %rdx
    syscall

    # Restore stack registers and return the original input
    movq %rbp, %rsp
    popq %rbp
    movq %r10, %rax
    ret


# ***** Function 'print_bool' *****
# Prints either 'true' or 'false', followed by a newline.
print_bool:
    pushq %rbp               # Save previous stack frame pointer
    movq %rsp, %rbp          # Set stack frame pointer
    movq %rdi, %r10          # Back up original input

    cmpq $0, %rdi            # See if the argument is false (i.e. 0)
    jne .Ltrue
    movq $false_str, %rsi    # If so, set %rsi to the address of the string for false
    movq $false_str_len, %rdx       # and %rdx to the length of that string,
    jmp .Lwrite
.Ltrue:
    movq $true_str, %rsi     # otherwise do the same with the string for true.
    movq $true_str_len, %rdx

.Lwrite:
    # Call syscall 'write'
    movq $1, %rax            # rax = syscall number for write
    movq $1, %rdi            # rdi = file handle for stdout
    # rsi = pointer to message (already set above)
    # rdx = number of bytes (already set above)
    syscall
    
    # Restore stack registers and return the original input
    movq %rbp, %rsp
    popq %rbp
    movq %r10, %rax
    ret

true_str:
    .ascii "true\n"
true_str_len = . - true_str
false_str:
    .ascii "false\n"
false_str_len = . - false_str

# ***** Function 'read_int' *****
# Reads an integer from stdin, skipping non-digit characters, until a newline.
#
# To avoid the complexity of buffering, it very inefficiently
# makes a syscall to read each byte.
#
# It crashes the program if input could not be read.
read_int:
    pushq %rbp           # Save previous stack frame pointer
    movq %rsp, %rbp      # Set stack frame pointer
    pushq %r12           # Back up r12 since it's callee-saved
    pushq $0             # Reserve space for input
                         # (we only write the lowest byte,
                         # but loading 64-bits at once is easier)

    xorq %r9, %r9        # Clear r9 - it'll store the minus sign
    xorq %r10, %r10      # Clear r10 - it'll accumulate our output
                         # Skip r11 - syscalls destroy it
    xorq %r12, %r12      # Clear r12 - it'll count the number of input bytes read.

    # Loop until a newline or end of input is encountered
.Lloop:
    # Call syscall 'read'
    xorq %rax, %rax      # syscall number for read = 0
    xorq %rdi, %rdi      # file handle for stdin = 0
    movq %rsp, %rsi      # rsi = pointer to buffer
    movq $1, %rdx        # rdx = buffer size
    syscall              # result in rax = number of bytes read,
                         # or 0 on end of input, -1 on error

    # Check return value: either -1, 0 or 1.
    cmpq $0, %rax
    jg .Lno_error
    je .Lend_of_input
    jmp .Lerror

.Lend_of_input:
    cmpq $0, %r12
    je .Lerror           # If we've read no input, it's an error.
    jmp .Lend            # Otherwise complete reading this input.

.Lno_error:
    incq %r12            # Increment input byte counter
    movq (%rsp), %r8     # Load input byte to r8

    # If the input byte is 10 (newline), exit the loop
    cmpq $10, %r8
    je .Lend

    # If the input byte is 45 (minus sign), negate r9
    cmpq $45, %r8
    jne .Lnegation_done
    xorq $1, %r9
.Lnegation_done:

    # If the input byte is not between 48 ('0') and 57 ('9')
    # then skip it as a junk character.
    cmpq $48, %r8
    jl .Lloop
    cmpq $57, %r8
    jg .Lloop

    # Subtract 48 to get a digit 0..9
    subq $48, %r8

    # Shift the digit onto the result
    imulq $10, %r10
    addq %r8, %r10

    jmp .Lloop

.Lend:
    # If it's a negative number, negate the result
    cmpq $0, %r9
    je .Lfinal_negation_done
    neg %r10
.Lfinal_negation_done:
    # Restore stack registers and return the result
    popq %r12
    movq %rbp, %rsp
    popq %rbp
    movq %r10, %rax
    ret

.Lerror:
    # Write error message to stderr with syscall 'write'
    movq $1, %rax
    movq $2, %rdi
    movq $read_int_error_str, %rsi
    movq $read_int_error_str_len, %rdx
    syscall

    # Exit the program
    movq $60, %rax      # Syscall number for exit = 60.
    movq $1, %rdi       # Set exit code 1.
    syscall

read_int_error_str:
    .ascii "Error: read_int() failed to read input\n"
read_int_error_str_len = . - read_int_error_str


main:
pushq %rbp
movq %rsp, %rbp

# START

subq $176, %rsp

# Call(read_int, [], x1)
subq $8, %rsp
callq read_int
movq %rax, -8(%rbp)
addq $8, %rsp

# Copy(x1, x2)
movq -8(%rbp), %rax
movq %rax, -16(%rbp)

# Call(print_int, [x2], x3)
subq $8, %rsp
movq -16(%rbp), %rdi
callq print_int
movq %rax, -24(%rbp)
addq $8, %rsp

.L4:

# LoadIntConst(1, x7)
movq $1, -32(%rbp)
movq -32(%rbp), %rdi

# Call(>, [x2, x7], x8)
xor %rax, %rax
movq -16(%rbp), %rdx
cmpq -32(%rbp), %rdx
setg %al
movq %rax, -40(%rbp)

cmpq $0, -40(%rbp)
jne .L5
jmp .L6

.L5:

# LoadIntConst(2, x12)
movq $2, -48(%rbp)
movq -48(%rbp), %rdi

# Call(%, [x2, x12], x13)
movq -16(%rbp), %rax
cqto
idivq -48(%rbp)
movq %rdx, -56(%rbp)

# LoadIntConst(0, x14)
movq $0, -64(%rbp)
movq -64(%rbp), %rdi

# Call(==, [x13, x14], x15)
xor %rax, %rax
movq -56(%rbp), %rdx
cmpq -64(%rbp), %rdx
sete %al
movq %rax, -72(%rbp)

cmpq $0, -72(%rbp)
jne .L9
jmp .L10

.L9:

# LoadIntConst(2, x17)
movq $2, -80(%rbp)
movq -80(%rbp), %rdi

# Call(/, [x2, x17], x18)
movq -16(%rbp), %rax
cqto
idivq -80(%rbp)
movq %rax, -88(%rbp)

# Copy(x18, x2)
movq -88(%rbp), %rax
movq %rax, -16(%rbp)

# LoadIntConst(0, x19)
movq $0, -96(%rbp)
movq -96(%rbp), %rdi

# Copy(unit, x16)
movq -104(%rbp), %rax
movq %rax, -112(%rbp)

jmp .L11

.L10:

# LoadIntConst(3, x20)
movq $3, -120(%rbp)
movq -120(%rbp), %rdi

# Call(*, [x20, x2], x21)
movq -120(%rbp), %rax
imulq -16(%rbp), %rax
movq %rax, -128(%rbp)

# LoadIntConst(1, x22)
movq $1, -136(%rbp)
movq -136(%rbp), %rdi

# Call(+, [x21, x22], x23)
movq -128(%rbp), %rax
addq -136(%rbp), %rax
movq %rax, -144(%rbp)

# Copy(x23, x2)
movq -144(%rbp), %rax
movq %rax, -16(%rbp)

# LoadIntConst(0, x24)
movq $0, -152(%rbp)
movq -152(%rbp), %rdi

# Copy(unit, x16)
movq -104(%rbp), %rax
movq %rax, -112(%rbp)

.L11:

# Call(print_int, [x2], x25)
subq $8, %rsp
movq -16(%rbp), %rdi
callq print_int
movq %rax, -160(%rbp)
addq $8, %rsp

# LoadIntConst(0, x26)
movq $0, -168(%rbp)
movq -168(%rbp), %rdi

jmp .L4

.L6:

# LoadIntConst(0, x27)
movq $0, -176(%rbp)
movq -176(%rbp), %rdi


# END

movq $0, %rax
movq %rbp, %rsp
popq %rbp
ret
