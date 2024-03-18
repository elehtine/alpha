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

# LoadIntConst(1, x4)
movq $1, -8(%rbp)
movq -8(%rbp), %rdi

cmpq $0, -8(%rbp)
jne .L1
jmp .L2

.L1:

# LoadIntConst(1, x6)
movq $1, -16(%rbp)
movq -16(%rbp), %rdi

# Copy(x6, x5)
movq -16(%rbp), %rax
movq %rax, -24(%rbp)

jmp .L3

.L2:

# LoadIntConst(0, x7)
movq $0, -32(%rbp)
movq -32(%rbp), %rdi

# Copy(x7, x5)
movq -32(%rbp), %rax
movq %rax, -24(%rbp)

.L3:

# Call(print_int, [x5], x8)
subq $8, %rsp
movq -24(%rbp), %rdi
callq print_int
movq %rax, -40(%rbp)
addq $8, %rsp

# LoadIntConst(0, x12)
movq $0, -48(%rbp)
movq -48(%rbp), %rdi

cmpq $0, -48(%rbp)
jne .L9
jmp .L10

.L9:

# LoadIntConst(1, x14)
movq $1, -56(%rbp)
movq -56(%rbp), %rdi

# Copy(x14, x13)
movq -56(%rbp), %rax
movq %rax, -64(%rbp)

jmp .L11

.L10:

# LoadIntConst(0, x15)
movq $0, -72(%rbp)
movq -72(%rbp), %rdi

# Copy(x15, x13)
movq -72(%rbp), %rax
movq %rax, -64(%rbp)

.L11:

# Call(print_int, [x13], x16)
subq $8, %rsp
movq -64(%rbp), %rdi
callq print_int
movq %rax, -80(%rbp)
addq $8, %rsp

# LoadIntConst(1, x20)
movq $1, -88(%rbp)
movq -88(%rbp), %rdi

# LoadIntConst(2, x21)
movq $2, -96(%rbp)
movq -96(%rbp), %rdi

# Call(<, [x20, x21], x22)
xor %rax, %rax
movq -88(%rbp), %rdx
cmpq -96(%rbp), %rdx
setl %al
movq %rax, -104(%rbp)

cmpq $0, -104(%rbp)
jne .L17
jmp .L18

.L17:

# LoadIntConst(1, x24)
movq $1, -112(%rbp)
movq -112(%rbp), %rdi

# Copy(x24, x23)
movq -112(%rbp), %rax
movq %rax, -120(%rbp)

jmp .L19

.L18:

# LoadIntConst(2, x25)
movq $2, -128(%rbp)
movq -128(%rbp), %rdi

# Copy(x25, x23)
movq -128(%rbp), %rax
movq %rax, -120(%rbp)

.L19:

# Call(print_int, [x23], x26)
subq $8, %rsp
movq -120(%rbp), %rdi
callq print_int
movq %rax, -136(%rbp)
addq $8, %rsp

# LoadIntConst(1, x30)
movq $1, -144(%rbp)
movq -144(%rbp), %rdi

# LoadIntConst(2, x31)
movq $2, -152(%rbp)
movq -152(%rbp), %rdi

# Call(<, [x30, x31], x32)
xor %rax, %rax
movq -144(%rbp), %rdx
cmpq -152(%rbp), %rdx
setl %al
movq %rax, -160(%rbp)

# Call(not, [x32], x33)
movq -160(%rbp), %rax
xorq $1, %rax
movq %rax, -168(%rbp)

cmpq $0, -168(%rbp)
jne .L27
jmp .L28

.L27:

# LoadIntConst(1, x35)
movq $1, -176(%rbp)
movq -176(%rbp), %rdi

# Copy(x35, x34)
movq -176(%rbp), %rax
movq %rax, -184(%rbp)

jmp .L29

.L28:

# LoadIntConst(2, x36)
movq $2, -192(%rbp)
movq -192(%rbp), %rdi

# Copy(x36, x34)
movq -192(%rbp), %rax
movq %rax, -184(%rbp)

.L29:

# Call(print_int, [x34], x37)
subq $8, %rsp
movq -184(%rbp), %rdi
callq print_int
movq %rax, -200(%rbp)
addq $8, %rsp

# LoadIntConst(1, x41)
movq $1, -208(%rbp)
movq -208(%rbp), %rdi

# LoadIntConst(2, x42)
movq $2, -216(%rbp)
movq -216(%rbp), %rdi

# Call(<, [x41, x42], x43)
xor %rax, %rax
movq -208(%rbp), %rdx
cmpq -216(%rbp), %rdx
setl %al
movq %rax, -224(%rbp)

# LoadIntConst(3, x44)
movq $3, -232(%rbp)
movq -232(%rbp), %rdi

# LoadIntConst(3, x45)
movq $3, -240(%rbp)
movq -240(%rbp), %rdi

# Call(<=, [x44, x45], x46)
xor %rax, %rax
movq -232(%rbp), %rdx
cmpq -240(%rbp), %rdx
setle %al
movq %rax, -248(%rbp)

# Call(and, [x43, x46], x47)

cmpq $0, -256(%rbp)
jne .L38
jmp .L39

.L38:

# LoadIntConst(1, x49)
movq $1, -264(%rbp)
movq -264(%rbp), %rdi

# Copy(x49, x48)
movq -264(%rbp), %rax
movq %rax, -272(%rbp)

jmp .L40

.L39:

# LoadIntConst(2, x50)
movq $2, -280(%rbp)
movq -280(%rbp), %rdi

# Copy(x50, x48)
movq -280(%rbp), %rax
movq %rax, -272(%rbp)

.L40:

# Call(print_int, [x48], x51)
subq $8, %rsp
movq -272(%rbp), %rdi
callq print_int
movq %rax, -288(%rbp)
addq $8, %rsp

# LoadIntConst(1, x55)
movq $1, -296(%rbp)
movq -296(%rbp), %rdi

# LoadIntConst(2, x56)
movq $2, -304(%rbp)
movq -304(%rbp), %rdi

# Call(<, [x55, x56], x57)
xor %rax, %rax
movq -296(%rbp), %rdx
cmpq -304(%rbp), %rdx
setl %al
movq %rax, -312(%rbp)

# LoadIntConst(3, x58)
movq $3, -320(%rbp)
movq -320(%rbp), %rdi

# LoadIntConst(3, x59)
movq $3, -328(%rbp)
movq -328(%rbp), %rdi

# Call(<=, [x58, x59], x60)
xor %rax, %rax
movq -320(%rbp), %rdx
cmpq -328(%rbp), %rdx
setle %al
movq %rax, -336(%rbp)

# Call(or, [x57, x60], x61)

cmpq $0, -344(%rbp)
jne .L52
jmp .L53

.L52:

# LoadIntConst(1, x63)
movq $1, -352(%rbp)
movq -352(%rbp), %rdi

# Copy(x63, x62)
movq -352(%rbp), %rax
movq %rax, -360(%rbp)

jmp .L54

.L53:

# LoadIntConst(2, x64)
movq $2, -368(%rbp)
movq -368(%rbp), %rdi

# Copy(x64, x62)
movq -368(%rbp), %rax
movq %rax, -360(%rbp)

.L54:

# Call(print_int, [x62], x65)
subq $8, %rsp
movq -360(%rbp), %rdi
callq print_int
movq %rax, -376(%rbp)
addq $8, %rsp

# LoadIntConst(1, x69)
movq $1, -384(%rbp)
movq -384(%rbp), %rdi

# LoadIntConst(2, x70)
movq $2, -392(%rbp)
movq -392(%rbp), %rdi

# Call(<=, [x69, x70], x71)
xor %rax, %rax
movq -384(%rbp), %rdx
cmpq -392(%rbp), %rdx
setle %al
movq %rax, -400(%rbp)

# LoadIntConst(3, x72)
movq $3, -408(%rbp)
movq -408(%rbp), %rdi

# LoadIntConst(3, x73)
movq $3, -416(%rbp)
movq -416(%rbp), %rdi

# Call(<, [x72, x73], x74)
xor %rax, %rax
movq -408(%rbp), %rdx
cmpq -416(%rbp), %rdx
setl %al
movq %rax, -424(%rbp)

# Call(and, [x71, x74], x75)

cmpq $0, -432(%rbp)
jne .L66
jmp .L67

.L66:

# LoadIntConst(1, x77)
movq $1, -440(%rbp)
movq -440(%rbp), %rdi

# Copy(x77, x76)
movq -440(%rbp), %rax
movq %rax, -448(%rbp)

jmp .L68

.L67:

# LoadIntConst(2, x78)
movq $2, -456(%rbp)
movq -456(%rbp), %rdi

# Copy(x78, x76)
movq -456(%rbp), %rax
movq %rax, -448(%rbp)

.L68:

# Call(print_int, [x76], x79)
subq $8, %rsp
movq -448(%rbp), %rdi
callq print_int
movq %rax, -464(%rbp)
addq $8, %rsp

# LoadIntConst(1, x83)
movq $1, -472(%rbp)
movq -472(%rbp), %rdi

# LoadIntConst(2, x84)
movq $2, -480(%rbp)
movq -480(%rbp), %rdi

# Call(<=, [x83, x84], x85)
xor %rax, %rax
movq -472(%rbp), %rdx
cmpq -480(%rbp), %rdx
setle %al
movq %rax, -488(%rbp)

# LoadIntConst(3, x86)
movq $3, -496(%rbp)
movq -496(%rbp), %rdi

# LoadIntConst(3, x87)
movq $3, -504(%rbp)
movq -504(%rbp), %rdi

# Call(<, [x86, x87], x88)
xor %rax, %rax
movq -496(%rbp), %rdx
cmpq -504(%rbp), %rdx
setl %al
movq %rax, -512(%rbp)

# Call(or, [x85, x88], x89)

cmpq $0, -520(%rbp)
jne .L80
jmp .L81

.L80:

# LoadIntConst(1, x91)
movq $1, -528(%rbp)
movq -528(%rbp), %rdi

# Copy(x91, x90)
movq -528(%rbp), %rax
movq %rax, -536(%rbp)

jmp .L82

.L81:

# LoadIntConst(2, x92)
movq $2, -544(%rbp)
movq -544(%rbp), %rdi

# Copy(x92, x90)
movq -544(%rbp), %rax
movq %rax, -536(%rbp)

.L82:

# Call(print_int, [x90], x93)
subq $8, %rsp
movq -536(%rbp), %rdi
callq print_int
movq %rax, -552(%rbp)
addq $8, %rsp

# LoadIntConst(1, x97)
movq $1, -560(%rbp)
movq -560(%rbp), %rdi

# LoadIntConst(2, x98)
movq $2, -568(%rbp)
movq -568(%rbp), %rdi

# Call(==, [x97, x98], x99)
xor %rax, %rax
movq -560(%rbp), %rdx
cmpq -568(%rbp), %rdx
sete %al
movq %rax, -576(%rbp)

cmpq $0, -576(%rbp)
jne .L94
jmp .L95

.L94:

# LoadIntConst(3, x101)
movq $3, -584(%rbp)
movq -584(%rbp), %rdi

# Copy(x101, x100)
movq -584(%rbp), %rax
movq %rax, -592(%rbp)

jmp .L96

.L95:

# LoadIntConst(4, x102)
movq $4, -600(%rbp)
movq -600(%rbp), %rdi

# Copy(x102, x100)
movq -600(%rbp), %rax
movq %rax, -592(%rbp)

.L96:

# Call(print_int, [x100], x103)
subq $8, %rsp
movq -592(%rbp), %rdi
callq print_int
movq %rax, -608(%rbp)
addq $8, %rsp

# LoadIntConst(1, x107)
movq $1, -616(%rbp)
movq -616(%rbp), %rdi

# LoadIntConst(2, x108)
movq $2, -624(%rbp)
movq -624(%rbp), %rdi

# Call(!=, [x107, x108], x109)
xor %rax, %rax
movq -616(%rbp), %rdx
cmpq -624(%rbp), %rdx
setne %al
movq %rax, -632(%rbp)

cmpq $0, -632(%rbp)
jne .L104
jmp .L105

.L104:

# LoadIntConst(3, x111)
movq $3, -640(%rbp)
movq -640(%rbp), %rdi

# Copy(x111, x110)
movq -640(%rbp), %rax
movq %rax, -648(%rbp)

jmp .L106

.L105:

# LoadIntConst(4, x112)
movq $4, -656(%rbp)
movq -656(%rbp), %rdi

# Copy(x112, x110)
movq -656(%rbp), %rax
movq %rax, -648(%rbp)

.L106:

# Call(print_int, [x110], x113)
subq $8, %rsp
movq -648(%rbp), %rdi
callq print_int
movq %rax, -664(%rbp)
addq $8, %rsp

# LoadIntConst(0, x117)
movq $0, -672(%rbp)
movq -672(%rbp), %rdi

# LoadIntConst(1, x118)
movq $1, -680(%rbp)
movq -680(%rbp), %rdi

# Call(and, [x117, x118], x119)

# LoadIntConst(1, x120)
movq $1, -696(%rbp)
movq -696(%rbp), %rdi

# Call(or, [x119, x120], x121)

cmpq $0, -704(%rbp)
jne .L114
jmp .L115

.L114:

# LoadIntConst(1, x123)
movq $1, -712(%rbp)
movq -712(%rbp), %rdi

# Copy(x123, x122)
movq -712(%rbp), %rax
movq %rax, -720(%rbp)

jmp .L116

.L115:

# LoadIntConst(2, x124)
movq $2, -728(%rbp)
movq -728(%rbp), %rdi

# Copy(x124, x122)
movq -728(%rbp), %rax
movq %rax, -720(%rbp)

.L116:

# Call(print_int, [x122], x125)
subq $8, %rsp
movq -720(%rbp), %rdi
callq print_int
movq %rax, -736(%rbp)
addq $8, %rsp

# LoadIntConst(0, x126)
movq $0, -744(%rbp)
movq -744(%rbp), %rdi


# END

movq $0, %rax
movq %rbp, %rsp
popq %rbp
ret
