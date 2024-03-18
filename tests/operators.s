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

subq $696, %rsp
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

# LoadIntConst(1, x39)
movq $1, -288(%rbp)
movq -288(%rbp), %rdi

# Call(print_int, [x39], x40)
subq $8, %rsp
movq -288(%rbp), %rdi
callq print_int
movq %rax, -296(%rbp)
addq $8, %rsp

jmp .L35

.L34:

# LoadIntConst(2, x41)
movq $2, -304(%rbp)
movq -304(%rbp), %rdi

# Call(print_int, [x41], x42)
subq $8, %rsp
movq -304(%rbp), %rdi
callq print_int
movq %rax, -312(%rbp)
addq $8, %rsp

.L35:

# LoadIntConst(1, x46)
movq $1, -320(%rbp)
movq -320(%rbp), %rdi

# LoadIntConst(2, x47)
movq $2, -328(%rbp)
movq -328(%rbp), %rdi

# Call(>, [x46, x47], x48)
xor %rax, %rax
movq -320(%rbp), %rdx
cmpq -328(%rbp), %rdx
setg %al
movq %rax, -336(%rbp)

cmpq $0, -336(%rbp)
jne .L43
jmp .L44

.L43:

# LoadIntConst(1, x49)
movq $1, -344(%rbp)
movq -344(%rbp), %rdi

# Call(print_int, [x49], x50)
subq $8, %rsp
movq -344(%rbp), %rdi
callq print_int
movq %rax, -352(%rbp)
addq $8, %rsp

jmp .L45

.L44:

# LoadIntConst(2, x51)
movq $2, -360(%rbp)
movq -360(%rbp), %rdi

# Call(print_int, [x51], x52)
subq $8, %rsp
movq -360(%rbp), %rdi
callq print_int
movq %rax, -368(%rbp)
addq $8, %rsp

.L45:

# LoadIntConst(1, x56)
movq $1, -376(%rbp)
movq -376(%rbp), %rdi

# LoadIntConst(1, x57)
movq $1, -384(%rbp)
movq -384(%rbp), %rdi

# Call(>=, [x56, x57], x58)
xor %rax, %rax
movq -376(%rbp), %rdx
cmpq -384(%rbp), %rdx
setge %al
movq %rax, -392(%rbp)

cmpq $0, -392(%rbp)
jne .L53
jmp .L54

.L53:

# LoadIntConst(1, x59)
movq $1, -400(%rbp)
movq -400(%rbp), %rdi

# Call(print_int, [x59], x60)
subq $8, %rsp
movq -400(%rbp), %rdi
callq print_int
movq %rax, -408(%rbp)
addq $8, %rsp

jmp .L55

.L54:

# LoadIntConst(2, x61)
movq $2, -416(%rbp)
movq -416(%rbp), %rdi

# Call(print_int, [x61], x62)
subq $8, %rsp
movq -416(%rbp), %rdi
callq print_int
movq %rax, -424(%rbp)
addq $8, %rsp

.L55:

# LoadIntConst(1, x66)
movq $1, -432(%rbp)
movq -432(%rbp), %rdi

# LoadIntConst(1, x67)
movq $1, -440(%rbp)
movq -440(%rbp), %rdi

# Call(<=, [x66, x67], x68)
xor %rax, %rax
movq -432(%rbp), %rdx
cmpq -440(%rbp), %rdx
setle %al
movq %rax, -448(%rbp)

cmpq $0, -448(%rbp)
jne .L63
jmp .L64

.L63:

# LoadIntConst(1, x69)
movq $1, -456(%rbp)
movq -456(%rbp), %rdi

# Call(print_int, [x69], x70)
subq $8, %rsp
movq -456(%rbp), %rdi
callq print_int
movq %rax, -464(%rbp)
addq $8, %rsp

jmp .L65

.L64:

# LoadIntConst(2, x71)
movq $2, -472(%rbp)
movq -472(%rbp), %rdi

# Call(print_int, [x71], x72)
subq $8, %rsp
movq -472(%rbp), %rdi
callq print_int
movq %rax, -480(%rbp)
addq $8, %rsp

.L65:

# LoadIntConst(3, x73)
movq $3, -488(%rbp)
movq -488(%rbp), %rdi

# LoadIntConst(1, x77)
movq $1, -496(%rbp)
movq -496(%rbp), %rdi

# LoadIntConst(2, x78)
movq $2, -504(%rbp)
movq -504(%rbp), %rdi

# Call(<, [x77, x78], x79)
xor %rax, %rax
movq -496(%rbp), %rdx
cmpq -504(%rbp), %rdx
setl %al
movq %rax, -512(%rbp)

cmpq $0, -512(%rbp)
jne .L74
jmp .L75

.L74:

# LoadIntConst(1, x80)
movq $1, -520(%rbp)
movq -520(%rbp), %rdi

jmp .L76

.L75:

# LoadIntConst(2, x81)
movq $2, -528(%rbp)
movq -528(%rbp), %rdi

.L76:

# Call(+, [x73, null], x82)
movq -488(%rbp), %rax
addq -536(%rbp), %rax
movq %rax, -544(%rbp)

# Call(print_int, [x82], x83)
subq $8, %rsp
movq -544(%rbp), %rdi
callq print_int
movq %rax, -552(%rbp)
addq $8, %rsp

# LoadIntConst(3, x84)
movq $3, -560(%rbp)
movq -560(%rbp), %rdi

# LoadIntConst(1, x88)
movq $1, -568(%rbp)
movq -568(%rbp), %rdi

# LoadIntConst(2, x89)
movq $2, -576(%rbp)
movq -576(%rbp), %rdi

# Call(>, [x88, x89], x90)
xor %rax, %rax
movq -568(%rbp), %rdx
cmpq -576(%rbp), %rdx
setg %al
movq %rax, -584(%rbp)

cmpq $0, -584(%rbp)
jne .L85
jmp .L86

.L85:

# LoadIntConst(1, x91)
movq $1, -592(%rbp)
movq -592(%rbp), %rdi

jmp .L87

.L86:

# LoadIntConst(2, x92)
movq $2, -600(%rbp)
movq -600(%rbp), %rdi

.L87:

# Call(+, [x84, null], x93)
movq -560(%rbp), %rax
addq -536(%rbp), %rax
movq %rax, -608(%rbp)

# Call(print_int, [x93], x94)
subq $8, %rsp
movq -608(%rbp), %rdi
callq print_int
movq %rax, -616(%rbp)
addq $8, %rsp

# LoadIntConst(1, x98)
movq $1, -624(%rbp)
movq -624(%rbp), %rdi

# Call(-, [x98], x99)
movq -624(%rbp), %rax
negq %rax
movq %rax, -632(%rbp)

# LoadIntConst(2, x100)
movq $2, -640(%rbp)
movq -640(%rbp), %rdi

# Call(>, [x99, x100], x101)
xor %rax, %rax
movq -632(%rbp), %rdx
cmpq -640(%rbp), %rdx
setg %al
movq %rax, -648(%rbp)

# Call(not, [x101], x102)
movq -648(%rbp), %rax
xorq $1, %rax
movq %rax, -656(%rbp)

cmpq $0, -656(%rbp)
jne .L95
jmp .L96

.L95:

# LoadIntConst(1, x103)
movq $1, -664(%rbp)
movq -664(%rbp), %rdi

jmp .L97

.L96:

# LoadIntConst(2, x104)
movq $2, -672(%rbp)
movq -672(%rbp), %rdi

.L97:

# Call(-, [null], x105)
movq -536(%rbp), %rax
negq %rax
movq %rax, -680(%rbp)

# Call(print_int, [x105], x106)
subq $8, %rsp
movq -680(%rbp), %rdi
callq print_int
movq %rax, -688(%rbp)
addq $8, %rsp

# LoadIntConst(0, x107)
movq $0, -696(%rbp)
movq -696(%rbp), %rdi


# END

movq $0, %rax
movq %rbp, %rsp
popq %rbp
ret
