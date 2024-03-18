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

subq $744, %rsp
subq $8, %rsp

# LoadIntConst(1, x1)
movq $1, -8(%rbp)
movq -8(%rbp), %rdi

# LoadIntConst(2, x2)
movq $2, -16(%rbp)
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

# LoadIntConst(4, x6)
movq $4, -48(%rbp)
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

# LoadIntConst(5, x9)
movq $5, -72(%rbp)
movq -72(%rbp), %rdi

# LoadIntConst(6, x10)
movq $6, -80(%rbp)
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

# LoadIntConst(7, x13)
movq $7, -104(%rbp)
movq -104(%rbp), %rdi

# LoadIntConst(2, x14)
movq $2, -112(%rbp)
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

# LoadIntConst(1, x17)
movq $1, -136(%rbp)
movq -136(%rbp), %rdi

# LoadIntConst(2, x18)
movq $2, -144(%rbp)
movq -144(%rbp), %rdi

# Call(+, [x17, x18], x19)
movq -136(%rbp), %rax
addq -144(%rbp), %rax
movq %rax, -152(%rbp)

# LoadIntConst(3, x20)
movq $3, -160(%rbp)
movq -160(%rbp), %rdi

# Call(*, [x19, x20], x21)
movq -152(%rbp), %rax
imulq -160(%rbp), %rax
movq %rax, -168(%rbp)

# Call(print_int, [x21], x22)
subq $8, %rsp
movq -168(%rbp), %rdi
callq print_int
movq %rax, -176(%rbp)
addq $8, %rsp

# LoadIntConst(1, x23)
movq $1, -184(%rbp)
movq -184(%rbp), %rdi

# LoadIntConst(2, x24)
movq $2, -192(%rbp)
movq -192(%rbp), %rdi

# LoadIntConst(3, x25)
movq $3, -200(%rbp)
movq -200(%rbp), %rdi

# Call(*, [x24, x25], x26)
movq -192(%rbp), %rax
imulq -200(%rbp), %rax
movq %rax, -208(%rbp)

# Call(+, [x23, x26], x27)
movq -184(%rbp), %rax
addq -208(%rbp), %rax
movq %rax, -216(%rbp)

# LoadIntConst(4, x28)
movq $4, -224(%rbp)
movq -224(%rbp), %rdi

# LoadIntConst(2, x29)
movq $2, -232(%rbp)
movq -232(%rbp), %rdi

# Call(/, [x28, x29], x30)
movq -224(%rbp), %rax
cqto
idivq -232(%rbp)
movq %rax, -240(%rbp)

# Call(-, [x27, x30], x31)
movq -216(%rbp), %rax
subq -240(%rbp), %rax
movq %rax, -248(%rbp)

# Call(print_int, [x31], x32)
subq $8, %rsp
movq -248(%rbp), %rdi
callq print_int
movq %rax, -256(%rbp)
addq $8, %rsp

# LoadIntConst(1, x36)
movq $1, -264(%rbp)
movq -264(%rbp), %rdi

# LoadIntConst(2, x37)
movq $2, -272(%rbp)
movq -272(%rbp), %rdi

# Call(<, [x36, x37], x38)
xor %rax, %rax
movq -264(%rbp), %rdx
cmpq -272(%rbp), %rdx
setl %al
movq %rax, -280(%rbp)

cmpq $0, -280(%rbp)
jne .L33
jmp .L34

.L33:

# LoadIntConst(1, x40)
movq $1, -288(%rbp)
movq -288(%rbp), %rdi

# Call(print_int, [x40], x41)
subq $8, %rsp
movq -288(%rbp), %rdi
callq print_int
movq %rax, -296(%rbp)
addq $8, %rsp

# Copy(x41, x39)
movq -296(%rbp), %rax
movq %rax, -304(%rbp)

jmp .L35

.L34:

# LoadIntConst(2, x42)
movq $2, -312(%rbp)
movq -312(%rbp), %rdi

# Call(print_int, [x42], x43)
subq $8, %rsp
movq -312(%rbp), %rdi
callq print_int
movq %rax, -320(%rbp)
addq $8, %rsp

# Copy(x43, x39)
movq -320(%rbp), %rax
movq %rax, -304(%rbp)

.L35:

# LoadIntConst(1, x47)
movq $1, -328(%rbp)
movq -328(%rbp), %rdi

# LoadIntConst(2, x48)
movq $2, -336(%rbp)
movq -336(%rbp), %rdi

# Call(>, [x47, x48], x49)
xor %rax, %rax
movq -328(%rbp), %rdx
cmpq -336(%rbp), %rdx
setg %al
movq %rax, -344(%rbp)

cmpq $0, -344(%rbp)
jne .L44
jmp .L45

.L44:

# LoadIntConst(1, x51)
movq $1, -352(%rbp)
movq -352(%rbp), %rdi

# Call(print_int, [x51], x52)
subq $8, %rsp
movq -352(%rbp), %rdi
callq print_int
movq %rax, -360(%rbp)
addq $8, %rsp

# Copy(x52, x50)
movq -360(%rbp), %rax
movq %rax, -368(%rbp)

jmp .L46

.L45:

# LoadIntConst(2, x53)
movq $2, -376(%rbp)
movq -376(%rbp), %rdi

# Call(print_int, [x53], x54)
subq $8, %rsp
movq -376(%rbp), %rdi
callq print_int
movq %rax, -384(%rbp)
addq $8, %rsp

# Copy(x54, x50)
movq -384(%rbp), %rax
movq %rax, -368(%rbp)

.L46:

# LoadIntConst(1, x58)
movq $1, -392(%rbp)
movq -392(%rbp), %rdi

# LoadIntConst(1, x59)
movq $1, -400(%rbp)
movq -400(%rbp), %rdi

# Call(>=, [x58, x59], x60)
xor %rax, %rax
movq -392(%rbp), %rdx
cmpq -400(%rbp), %rdx
setge %al
movq %rax, -408(%rbp)

cmpq $0, -408(%rbp)
jne .L55
jmp .L56

.L55:

# LoadIntConst(1, x62)
movq $1, -416(%rbp)
movq -416(%rbp), %rdi

# Call(print_int, [x62], x63)
subq $8, %rsp
movq -416(%rbp), %rdi
callq print_int
movq %rax, -424(%rbp)
addq $8, %rsp

# Copy(x63, x61)
movq -424(%rbp), %rax
movq %rax, -432(%rbp)

jmp .L57

.L56:

# LoadIntConst(2, x64)
movq $2, -440(%rbp)
movq -440(%rbp), %rdi

# Call(print_int, [x64], x65)
subq $8, %rsp
movq -440(%rbp), %rdi
callq print_int
movq %rax, -448(%rbp)
addq $8, %rsp

# Copy(x65, x61)
movq -448(%rbp), %rax
movq %rax, -432(%rbp)

.L57:

# LoadIntConst(1, x69)
movq $1, -456(%rbp)
movq -456(%rbp), %rdi

# LoadIntConst(1, x70)
movq $1, -464(%rbp)
movq -464(%rbp), %rdi

# Call(<=, [x69, x70], x71)
xor %rax, %rax
movq -456(%rbp), %rdx
cmpq -464(%rbp), %rdx
setle %al
movq %rax, -472(%rbp)

cmpq $0, -472(%rbp)
jne .L66
jmp .L67

.L66:

# LoadIntConst(1, x73)
movq $1, -480(%rbp)
movq -480(%rbp), %rdi

# Call(print_int, [x73], x74)
subq $8, %rsp
movq -480(%rbp), %rdi
callq print_int
movq %rax, -488(%rbp)
addq $8, %rsp

# Copy(x74, x72)
movq -488(%rbp), %rax
movq %rax, -496(%rbp)

jmp .L68

.L67:

# LoadIntConst(2, x75)
movq $2, -504(%rbp)
movq -504(%rbp), %rdi

# Call(print_int, [x75], x76)
subq $8, %rsp
movq -504(%rbp), %rdi
callq print_int
movq %rax, -512(%rbp)
addq $8, %rsp

# Copy(x76, x72)
movq -512(%rbp), %rax
movq %rax, -496(%rbp)

.L68:

# LoadIntConst(3, x77)
movq $3, -520(%rbp)
movq -520(%rbp), %rdi

# LoadIntConst(1, x81)
movq $1, -528(%rbp)
movq -528(%rbp), %rdi

# LoadIntConst(2, x82)
movq $2, -536(%rbp)
movq -536(%rbp), %rdi

# Call(<, [x81, x82], x83)
xor %rax, %rax
movq -528(%rbp), %rdx
cmpq -536(%rbp), %rdx
setl %al
movq %rax, -544(%rbp)

cmpq $0, -544(%rbp)
jne .L78
jmp .L79

.L78:

# LoadIntConst(1, x85)
movq $1, -552(%rbp)
movq -552(%rbp), %rdi

# Copy(x85, x84)
movq -552(%rbp), %rax
movq %rax, -560(%rbp)

jmp .L80

.L79:

# LoadIntConst(2, x86)
movq $2, -568(%rbp)
movq -568(%rbp), %rdi

# Copy(x86, x84)
movq -568(%rbp), %rax
movq %rax, -560(%rbp)

.L80:

# Call(+, [x77, x84], x87)
movq -520(%rbp), %rax
addq -560(%rbp), %rax
movq %rax, -576(%rbp)

# Call(print_int, [x87], x88)
subq $8, %rsp
movq -576(%rbp), %rdi
callq print_int
movq %rax, -584(%rbp)
addq $8, %rsp

# LoadIntConst(3, x89)
movq $3, -592(%rbp)
movq -592(%rbp), %rdi

# LoadIntConst(1, x93)
movq $1, -600(%rbp)
movq -600(%rbp), %rdi

# LoadIntConst(2, x94)
movq $2, -608(%rbp)
movq -608(%rbp), %rdi

# Call(>, [x93, x94], x95)
xor %rax, %rax
movq -600(%rbp), %rdx
cmpq -608(%rbp), %rdx
setg %al
movq %rax, -616(%rbp)

cmpq $0, -616(%rbp)
jne .L90
jmp .L91

.L90:

# LoadIntConst(1, x97)
movq $1, -624(%rbp)
movq -624(%rbp), %rdi

# Copy(x97, x96)
movq -624(%rbp), %rax
movq %rax, -632(%rbp)

jmp .L92

.L91:

# LoadIntConst(2, x98)
movq $2, -640(%rbp)
movq -640(%rbp), %rdi

# Copy(x98, x96)
movq -640(%rbp), %rax
movq %rax, -632(%rbp)

.L92:

# Call(+, [x89, x96], x99)
movq -592(%rbp), %rax
addq -632(%rbp), %rax
movq %rax, -648(%rbp)

# Call(print_int, [x99], x100)
subq $8, %rsp
movq -648(%rbp), %rdi
callq print_int
movq %rax, -656(%rbp)
addq $8, %rsp

# LoadIntConst(1, x104)
movq $1, -664(%rbp)
movq -664(%rbp), %rdi

# Call(-, [x104], x105)
movq -664(%rbp), %rax
negq %rax
movq %rax, -672(%rbp)

# LoadIntConst(2, x106)
movq $2, -680(%rbp)
movq -680(%rbp), %rdi

# Call(>, [x105, x106], x107)
xor %rax, %rax
movq -672(%rbp), %rdx
cmpq -680(%rbp), %rdx
setg %al
movq %rax, -688(%rbp)

# Call(not, [x107], x108)
movq -688(%rbp), %rax
xorq $1, %rax
movq %rax, -696(%rbp)

cmpq $0, -696(%rbp)
jne .L101
jmp .L102

.L101:

# LoadIntConst(1, x110)
movq $1, -704(%rbp)
movq -704(%rbp), %rdi

# Copy(x110, x109)
movq -704(%rbp), %rax
movq %rax, -712(%rbp)

jmp .L103

.L102:

# LoadIntConst(2, x111)
movq $2, -720(%rbp)
movq -720(%rbp), %rdi

# Copy(x111, x109)
movq -720(%rbp), %rax
movq %rax, -712(%rbp)

.L103:

# Call(-, [x109], x112)
movq -712(%rbp), %rax
negq %rax
movq %rax, -728(%rbp)

# Call(print_int, [x112], x113)
subq $8, %rsp
movq -728(%rbp), %rdi
callq print_int
movq %rax, -736(%rbp)
addq $8, %rsp

# LoadIntConst(0, x114)
movq $0, -744(%rbp)
movq -744(%rbp), %rdi


# END

movq $0, %rax
movq %rbp, %rsp
popq %rbp
ret
