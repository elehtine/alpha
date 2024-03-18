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

subq $408, %rsp
subq $8, %rsp

# LoadIntConst(3, x1)
movq $3, -8(%rbp)
movq -8(%rbp), %rdi

# LoadIntConst(5, x2)
movq $5, -16(%rbp)
movq -16(%rbp), %rdi

# Call(+, [x1, x2], x3)
movq -8(%rbp), %rax
addq -16(%rbp), %rax
movq %rax, -24(%rbp)

# Call(print_int, [x3], x4)
subq $8, %rsp
movq -24(%rbp), %rdi
callq print_int
movq %rax, -32(%rbp)
addq $8, %rsp

# LoadIntConst(3, x5)
movq $3, -40(%rbp)
movq -40(%rbp), %rdi

# LoadIntConst(5, x6)
movq $5, -48(%rbp)
movq -48(%rbp), %rdi

# Call(-, [x5, x6], x7)
movq -40(%rbp), %rax
subq -48(%rbp), %rax
movq %rax, -56(%rbp)

# Call(print_int, [x7], x8)
subq $8, %rsp
movq -56(%rbp), %rdi
callq print_int
movq %rax, -64(%rbp)
addq $8, %rsp

# LoadIntConst(3, x9)
movq $3, -72(%rbp)
movq -72(%rbp), %rdi

# LoadIntConst(5, x10)
movq $5, -80(%rbp)
movq -80(%rbp), %rdi

# Call(*, [x9, x10], x11)
movq -72(%rbp), %rax
imulq -80(%rbp), %rax
movq %rax, -88(%rbp)

# Call(print_int, [x11], x12)
subq $8, %rsp
movq -88(%rbp), %rdi
callq print_int
movq %rax, -96(%rbp)
addq $8, %rsp

# LoadIntConst(3, x13)
movq $3, -104(%rbp)
movq -104(%rbp), %rdi

# LoadIntConst(5, x14)
movq $5, -112(%rbp)
movq -112(%rbp), %rdi

# Call(/, [x13, x14], x15)
movq -104(%rbp), %rax
cqto
idivq -112(%rbp)
movq %rax, -120(%rbp)

# Call(print_int, [x15], x16)
subq $8, %rsp
movq -120(%rbp), %rdi
callq print_int
movq %rax, -128(%rbp)
addq $8, %rsp

# LoadIntConst(3, x17)
movq $3, -136(%rbp)
movq -136(%rbp), %rdi

# LoadIntConst(5, x18)
movq $5, -144(%rbp)
movq -144(%rbp), %rdi

# Call(%, [x17, x18], x19)
movq -136(%rbp), %rax
cqto
idivq -144(%rbp)
movq %rdx, -152(%rbp)

# Call(print_int, [x19], x20)
subq $8, %rsp
movq -152(%rbp), %rdi
callq print_int
movq %rax, -160(%rbp)
addq $8, %rsp

# LoadIntConst(5, x21)
movq $5, -168(%rbp)
movq -168(%rbp), %rdi

# Call(-, [x21], x22)
movq -168(%rbp), %rax
negq %rax
movq %rax, -176(%rbp)

# Call(print_int, [x22], x23)
subq $8, %rsp
movq -176(%rbp), %rdi
callq print_int
movq %rax, -184(%rbp)
addq $8, %rsp

# LoadIntConst(1, x24)
movq $1, -192(%rbp)
movq -192(%rbp), %rdi

# Call(not, [x24], x25)
movq -192(%rbp), %rax
xorq $1, %rax
movq %rax, -200(%rbp)

# Call(print_bool, [x25], x26)
subq $8, %rsp
movq -200(%rbp), %rdi
callq print_bool
movq %rax, -208(%rbp)
addq $8, %rsp

# LoadIntConst(3, x27)
movq $3, -216(%rbp)
movq -216(%rbp), %rdi

# LoadIntConst(5, x28)
movq $5, -224(%rbp)
movq -224(%rbp), %rdi

# Call(==, [x27, x28], x29)
xor %rax, %rax
movq -216(%rbp), %rdx
cmpq -224(%rbp), %rdx
sete %al
movq %rax, -232(%rbp)

# Call(print_bool, [x29], x30)
subq $8, %rsp
movq -232(%rbp), %rdi
callq print_bool
movq %rax, -240(%rbp)
addq $8, %rsp

# LoadIntConst(3, x31)
movq $3, -248(%rbp)
movq -248(%rbp), %rdi

# LoadIntConst(5, x32)
movq $5, -256(%rbp)
movq -256(%rbp), %rdi

# Call(!=, [x31, x32], x33)
xor %rax, %rax
movq -248(%rbp), %rdx
cmpq -256(%rbp), %rdx
setne %al
movq %rax, -264(%rbp)

# Call(print_bool, [x33], x34)
subq $8, %rsp
movq -264(%rbp), %rdi
callq print_bool
movq %rax, -272(%rbp)
addq $8, %rsp

# LoadIntConst(3, x35)
movq $3, -280(%rbp)
movq -280(%rbp), %rdi

# LoadIntConst(5, x36)
movq $5, -288(%rbp)
movq -288(%rbp), %rdi

# Call(<, [x35, x36], x37)
xor %rax, %rax
movq -280(%rbp), %rdx
cmpq -288(%rbp), %rdx
setl %al
movq %rax, -296(%rbp)

# Call(print_bool, [x37], x38)
subq $8, %rsp
movq -296(%rbp), %rdi
callq print_bool
movq %rax, -304(%rbp)
addq $8, %rsp

# LoadIntConst(3, x39)
movq $3, -312(%rbp)
movq -312(%rbp), %rdi

# LoadIntConst(5, x40)
movq $5, -320(%rbp)
movq -320(%rbp), %rdi

# Call(<=, [x39, x40], x41)
xor %rax, %rax
movq -312(%rbp), %rdx
cmpq -320(%rbp), %rdx
setle %al
movq %rax, -328(%rbp)

# Call(print_bool, [x41], x42)
subq $8, %rsp
movq -328(%rbp), %rdi
callq print_bool
movq %rax, -336(%rbp)
addq $8, %rsp

# LoadIntConst(3, x43)
movq $3, -344(%rbp)
movq -344(%rbp), %rdi

# LoadIntConst(5, x44)
movq $5, -352(%rbp)
movq -352(%rbp), %rdi

# Call(>, [x43, x44], x45)
xor %rax, %rax
movq -344(%rbp), %rdx
cmpq -352(%rbp), %rdx
setg %al
movq %rax, -360(%rbp)

# Call(print_bool, [x45], x46)
subq $8, %rsp
movq -360(%rbp), %rdi
callq print_bool
movq %rax, -368(%rbp)
addq $8, %rsp

# LoadIntConst(3, x47)
movq $3, -376(%rbp)
movq -376(%rbp), %rdi

# LoadIntConst(5, x48)
movq $5, -384(%rbp)
movq -384(%rbp), %rdi

# Call(>=, [x47, x48], x49)
xor %rax, %rax
movq -376(%rbp), %rdx
cmpq -384(%rbp), %rdx
setge %al
movq %rax, -392(%rbp)

# Call(print_bool, [x49], x50)
subq $8, %rsp
movq -392(%rbp), %rdi
callq print_bool
movq %rax, -400(%rbp)
addq $8, %rsp

# LoadIntConst(0, x51)
movq $0, -408(%rbp)
movq -408(%rbp), %rdi


# END

movq $0, %rax
movq %rbp, %rsp
popq %rbp
ret
