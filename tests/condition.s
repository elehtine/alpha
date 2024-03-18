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

# LoadIntConst(1, x44)
movq $1, -208(%rbp)
movq -208(%rbp), %rdi

# LoadIntConst(2, x45)
movq $2, -216(%rbp)
movq -216(%rbp), %rdi

# Call(<, [x44, x45], x46)
xor %rax, %rax
movq -208(%rbp), %rdx
cmpq -216(%rbp), %rdx
setl %al
movq %rax, -224(%rbp)

# Copy(x46, x41)
movq -224(%rbp), %rax
movq %rax, -232(%rbp)

cmpq $0, -224(%rbp)
jne .L42
jmp .L43

.L42:

# LoadIntConst(3, x47)
movq $3, -240(%rbp)
movq -240(%rbp), %rdi

# LoadIntConst(3, x48)
movq $3, -248(%rbp)
movq -248(%rbp), %rdi

# Call(<=, [x47, x48], x49)
xor %rax, %rax
movq -240(%rbp), %rdx
cmpq -248(%rbp), %rdx
setle %al
movq %rax, -256(%rbp)

# Copy(x49, x41)
movq -256(%rbp), %rax
movq %rax, -232(%rbp)

.L43:

cmpq $0, -232(%rbp)
jne .L38
jmp .L39

.L38:

# LoadIntConst(1, x51)
movq $1, -264(%rbp)
movq -264(%rbp), %rdi

# Copy(x51, x50)
movq -264(%rbp), %rax
movq %rax, -272(%rbp)

jmp .L40

.L39:

# LoadIntConst(2, x52)
movq $2, -280(%rbp)
movq -280(%rbp), %rdi

# Copy(x52, x50)
movq -280(%rbp), %rax
movq %rax, -272(%rbp)

.L40:

# Call(print_int, [x50], x53)
subq $8, %rsp
movq -272(%rbp), %rdi
callq print_int
movq %rax, -288(%rbp)
addq $8, %rsp

# LoadIntConst(1, x60)
movq $1, -296(%rbp)
movq -296(%rbp), %rdi

# LoadIntConst(2, x61)
movq $2, -304(%rbp)
movq -304(%rbp), %rdi

# Call(<, [x60, x61], x62)
xor %rax, %rax
movq -296(%rbp), %rdx
cmpq -304(%rbp), %rdx
setl %al
movq %rax, -312(%rbp)

# Copy(x62, x57)
movq -312(%rbp), %rax
movq %rax, -320(%rbp)

cmpq $0, -312(%rbp)
jne .L59
jmp .L58

.L58:

# LoadIntConst(3, x63)
movq $3, -328(%rbp)
movq -328(%rbp), %rdi

# LoadIntConst(3, x64)
movq $3, -336(%rbp)
movq -336(%rbp), %rdi

# Call(<=, [x63, x64], x65)
xor %rax, %rax
movq -328(%rbp), %rdx
cmpq -336(%rbp), %rdx
setle %al
movq %rax, -344(%rbp)

# Copy(x65, x57)
movq -344(%rbp), %rax
movq %rax, -320(%rbp)

.L59:

cmpq $0, -320(%rbp)
jne .L54
jmp .L55

.L54:

# LoadIntConst(1, x67)
movq $1, -352(%rbp)
movq -352(%rbp), %rdi

# Copy(x67, x66)
movq -352(%rbp), %rax
movq %rax, -360(%rbp)

jmp .L56

.L55:

# LoadIntConst(2, x68)
movq $2, -368(%rbp)
movq -368(%rbp), %rdi

# Copy(x68, x66)
movq -368(%rbp), %rax
movq %rax, -360(%rbp)

.L56:

# Call(print_int, [x66], x69)
subq $8, %rsp
movq -360(%rbp), %rdi
callq print_int
movq %rax, -376(%rbp)
addq $8, %rsp

# LoadIntConst(1, x76)
movq $1, -384(%rbp)
movq -384(%rbp), %rdi

# LoadIntConst(2, x77)
movq $2, -392(%rbp)
movq -392(%rbp), %rdi

# Call(<=, [x76, x77], x78)
xor %rax, %rax
movq -384(%rbp), %rdx
cmpq -392(%rbp), %rdx
setle %al
movq %rax, -400(%rbp)

# Copy(x78, x73)
movq -400(%rbp), %rax
movq %rax, -408(%rbp)

cmpq $0, -400(%rbp)
jne .L74
jmp .L75

.L74:

# LoadIntConst(3, x79)
movq $3, -416(%rbp)
movq -416(%rbp), %rdi

# LoadIntConst(3, x80)
movq $3, -424(%rbp)
movq -424(%rbp), %rdi

# Call(<, [x79, x80], x81)
xor %rax, %rax
movq -416(%rbp), %rdx
cmpq -424(%rbp), %rdx
setl %al
movq %rax, -432(%rbp)

# Copy(x81, x73)
movq -432(%rbp), %rax
movq %rax, -408(%rbp)

.L75:

cmpq $0, -408(%rbp)
jne .L70
jmp .L71

.L70:

# LoadIntConst(1, x83)
movq $1, -440(%rbp)
movq -440(%rbp), %rdi

# Copy(x83, x82)
movq -440(%rbp), %rax
movq %rax, -448(%rbp)

jmp .L72

.L71:

# LoadIntConst(2, x84)
movq $2, -456(%rbp)
movq -456(%rbp), %rdi

# Copy(x84, x82)
movq -456(%rbp), %rax
movq %rax, -448(%rbp)

.L72:

# Call(print_int, [x82], x85)
subq $8, %rsp
movq -448(%rbp), %rdi
callq print_int
movq %rax, -464(%rbp)
addq $8, %rsp

# LoadIntConst(1, x92)
movq $1, -472(%rbp)
movq -472(%rbp), %rdi

# LoadIntConst(2, x93)
movq $2, -480(%rbp)
movq -480(%rbp), %rdi

# Call(<=, [x92, x93], x94)
xor %rax, %rax
movq -472(%rbp), %rdx
cmpq -480(%rbp), %rdx
setle %al
movq %rax, -488(%rbp)

# Copy(x94, x89)
movq -488(%rbp), %rax
movq %rax, -496(%rbp)

cmpq $0, -488(%rbp)
jne .L91
jmp .L90

.L90:

# LoadIntConst(3, x95)
movq $3, -504(%rbp)
movq -504(%rbp), %rdi

# LoadIntConst(3, x96)
movq $3, -512(%rbp)
movq -512(%rbp), %rdi

# Call(<, [x95, x96], x97)
xor %rax, %rax
movq -504(%rbp), %rdx
cmpq -512(%rbp), %rdx
setl %al
movq %rax, -520(%rbp)

# Copy(x97, x89)
movq -520(%rbp), %rax
movq %rax, -496(%rbp)

.L91:

cmpq $0, -496(%rbp)
jne .L86
jmp .L87

.L86:

# LoadIntConst(1, x99)
movq $1, -528(%rbp)
movq -528(%rbp), %rdi

# Copy(x99, x98)
movq -528(%rbp), %rax
movq %rax, -536(%rbp)

jmp .L88

.L87:

# LoadIntConst(2, x100)
movq $2, -544(%rbp)
movq -544(%rbp), %rdi

# Copy(x100, x98)
movq -544(%rbp), %rax
movq %rax, -536(%rbp)

.L88:

# Call(print_int, [x98], x101)
subq $8, %rsp
movq -536(%rbp), %rdi
callq print_int
movq %rax, -552(%rbp)
addq $8, %rsp

# LoadIntConst(1, x105)
movq $1, -560(%rbp)
movq -560(%rbp), %rdi

# LoadIntConst(2, x106)
movq $2, -568(%rbp)
movq -568(%rbp), %rdi

# Call(==, [x105, x106], x107)
xor %rax, %rax
movq -560(%rbp), %rdx
cmpq -568(%rbp), %rdx
sete %al
movq %rax, -576(%rbp)

cmpq $0, -576(%rbp)
jne .L102
jmp .L103

.L102:

# LoadIntConst(3, x109)
movq $3, -584(%rbp)
movq -584(%rbp), %rdi

# Copy(x109, x108)
movq -584(%rbp), %rax
movq %rax, -592(%rbp)

jmp .L104

.L103:

# LoadIntConst(4, x110)
movq $4, -600(%rbp)
movq -600(%rbp), %rdi

# Copy(x110, x108)
movq -600(%rbp), %rax
movq %rax, -592(%rbp)

.L104:

# Call(print_int, [x108], x111)
subq $8, %rsp
movq -592(%rbp), %rdi
callq print_int
movq %rax, -608(%rbp)
addq $8, %rsp

# LoadIntConst(1, x115)
movq $1, -616(%rbp)
movq -616(%rbp), %rdi

# LoadIntConst(2, x116)
movq $2, -624(%rbp)
movq -624(%rbp), %rdi

# Call(!=, [x115, x116], x117)
xor %rax, %rax
movq -616(%rbp), %rdx
cmpq -624(%rbp), %rdx
setne %al
movq %rax, -632(%rbp)

cmpq $0, -632(%rbp)
jne .L112
jmp .L113

.L112:

# LoadIntConst(3, x119)
movq $3, -640(%rbp)
movq -640(%rbp), %rdi

# Copy(x119, x118)
movq -640(%rbp), %rax
movq %rax, -648(%rbp)

jmp .L114

.L113:

# LoadIntConst(4, x120)
movq $4, -656(%rbp)
movq -656(%rbp), %rdi

# Copy(x120, x118)
movq -656(%rbp), %rax
movq %rax, -648(%rbp)

.L114:

# Call(print_int, [x118], x121)
subq $8, %rsp
movq -648(%rbp), %rdi
callq print_int
movq %rax, -664(%rbp)
addq $8, %rsp

# LoadIntConst(0, x131)
movq $0, -672(%rbp)
movq -672(%rbp), %rdi

# Copy(x131, x128)
movq -672(%rbp), %rax
movq %rax, -680(%rbp)

cmpq $0, -672(%rbp)
jne .L129
jmp .L130

.L129:

# LoadIntConst(1, x132)
movq $1, -688(%rbp)
movq -688(%rbp), %rdi

# Copy(x132, x128)
movq -688(%rbp), %rax
movq %rax, -680(%rbp)

.L130:

# Copy(x128, x125)
movq -680(%rbp), %rax
movq %rax, -696(%rbp)

cmpq $0, -680(%rbp)
jne .L127
jmp .L126

.L126:

# LoadIntConst(1, x133)
movq $1, -704(%rbp)
movq -704(%rbp), %rdi

# Copy(x133, x125)
movq -704(%rbp), %rax
movq %rax, -696(%rbp)

.L127:

cmpq $0, -696(%rbp)
jne .L122
jmp .L123

.L122:

# LoadIntConst(1, x135)
movq $1, -712(%rbp)
movq -712(%rbp), %rdi

# Copy(x135, x134)
movq -712(%rbp), %rax
movq %rax, -720(%rbp)

jmp .L124

.L123:

# LoadIntConst(2, x136)
movq $2, -728(%rbp)
movq -728(%rbp), %rdi

# Copy(x136, x134)
movq -728(%rbp), %rax
movq %rax, -720(%rbp)

.L124:

# Call(print_int, [x134], x137)
subq $8, %rsp
movq -720(%rbp), %rdi
callq print_int
movq %rax, -736(%rbp)
addq $8, %rsp

# LoadIntConst(0, x138)
movq $0, -744(%rbp)
movq -744(%rbp), %rdi


# END

movq $0, %rax
movq %rbp, %rsp
popq %rbp
ret
