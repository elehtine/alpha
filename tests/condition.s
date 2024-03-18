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

subq $600, %rsp
subq $8, %rsp

# LoadIntConst(1, x4)
movq $1, -8(%rbp)
movq -8(%rbp), %rdi

cmpq $0, -8(%rbp)
jne .L1
jmp .L2

.L1:

# LoadIntConst(1, x5)
movq $1, -16(%rbp)
movq -16(%rbp), %rdi

jmp .L3

.L2:

# LoadIntConst(0, x6)
movq $0, -24(%rbp)
movq -24(%rbp), %rdi

.L3:

# LoadIntConst(0, x10)
movq $0, -32(%rbp)
movq -32(%rbp), %rdi

cmpq $0, -32(%rbp)
jne .L7
jmp .L8

.L7:

# LoadIntConst(1, x11)
movq $1, -40(%rbp)
movq -40(%rbp), %rdi

jmp .L9

.L8:

# LoadIntConst(0, x12)
movq $0, -48(%rbp)
movq -48(%rbp), %rdi

.L9:

# LoadIntConst(1, x16)
movq $1, -56(%rbp)
movq -56(%rbp), %rdi

cmpq $0, -56(%rbp)
jne .L13
jmp .L14

.L13:

# LoadIntConst(1, x17)
movq $1, -64(%rbp)
movq -64(%rbp), %rdi

jmp .L15

.L14:

.L15:

# LoadIntConst(0, x21)
movq $0, -72(%rbp)
movq -72(%rbp), %rdi

cmpq $0, -72(%rbp)
jne .L18
jmp .L19

.L18:

# LoadIntConst(1, x22)
movq $1, -80(%rbp)
movq -80(%rbp), %rdi

jmp .L20

.L19:

.L20:

# LoadIntConst(1, x26)
movq $1, -88(%rbp)
movq -88(%rbp), %rdi

# LoadIntConst(2, x27)
movq $2, -96(%rbp)
movq -96(%rbp), %rdi

# Call(<, [x26, x27], x28)
xor %rax, %rax
movq -88(%rbp), %rdx
cmpq -96(%rbp), %rdx
setl %al
movq %rax, -104(%rbp)

cmpq $0, -104(%rbp)
jne .L23
jmp .L24

.L23:

# LoadIntConst(1, x29)
movq $1, -112(%rbp)
movq -112(%rbp), %rdi

jmp .L25

.L24:

# LoadIntConst(2, x30)
movq $2, -120(%rbp)
movq -120(%rbp), %rdi

.L25:

# LoadIntConst(1, x34)
movq $1, -128(%rbp)
movq -128(%rbp), %rdi

# LoadIntConst(2, x35)
movq $2, -136(%rbp)
movq -136(%rbp), %rdi

# Call(<, [x34, x35], x36)
xor %rax, %rax
movq -128(%rbp), %rdx
cmpq -136(%rbp), %rdx
setl %al
movq %rax, -144(%rbp)

# Call(not, [x36], x37)
movq -144(%rbp), %rax
xorq $1, %rax
movq %rax, -152(%rbp)

cmpq $0, -152(%rbp)
jne .L31
jmp .L32

.L31:

# LoadIntConst(1, x38)
movq $1, -160(%rbp)
movq -160(%rbp), %rdi

jmp .L33

.L32:

# LoadIntConst(2, x39)
movq $2, -168(%rbp)
movq -168(%rbp), %rdi

.L33:

# LoadIntConst(1, x43)
movq $1, -176(%rbp)
movq -176(%rbp), %rdi

# LoadIntConst(2, x44)
movq $2, -184(%rbp)
movq -184(%rbp), %rdi

# Call(<, [x43, x44], x45)
xor %rax, %rax
movq -176(%rbp), %rdx
cmpq -184(%rbp), %rdx
setl %al
movq %rax, -192(%rbp)

# LoadIntConst(3, x46)
movq $3, -200(%rbp)
movq -200(%rbp), %rdi

# LoadIntConst(3, x47)
movq $3, -208(%rbp)
movq -208(%rbp), %rdi

# Call(<=, [x46, x47], x48)
xor %rax, %rax
movq -200(%rbp), %rdx
cmpq -208(%rbp), %rdx
setle %al
movq %rax, -216(%rbp)

# Call(and, [x45, x48], x49)

cmpq $0, -224(%rbp)
jne .L40
jmp .L41

.L40:

# LoadIntConst(1, x50)
movq $1, -232(%rbp)
movq -232(%rbp), %rdi

jmp .L42

.L41:

# LoadIntConst(2, x51)
movq $2, -240(%rbp)
movq -240(%rbp), %rdi

.L42:

# LoadIntConst(1, x55)
movq $1, -248(%rbp)
movq -248(%rbp), %rdi

# LoadIntConst(2, x56)
movq $2, -256(%rbp)
movq -256(%rbp), %rdi

# Call(<, [x55, x56], x57)
xor %rax, %rax
movq -248(%rbp), %rdx
cmpq -256(%rbp), %rdx
setl %al
movq %rax, -264(%rbp)

# LoadIntConst(3, x58)
movq $3, -272(%rbp)
movq -272(%rbp), %rdi

# LoadIntConst(3, x59)
movq $3, -280(%rbp)
movq -280(%rbp), %rdi

# Call(<=, [x58, x59], x60)
xor %rax, %rax
movq -272(%rbp), %rdx
cmpq -280(%rbp), %rdx
setle %al
movq %rax, -288(%rbp)

# Call(or, [x57, x60], x61)

cmpq $0, -296(%rbp)
jne .L52
jmp .L53

.L52:

# LoadIntConst(1, x62)
movq $1, -304(%rbp)
movq -304(%rbp), %rdi

jmp .L54

.L53:

# LoadIntConst(2, x63)
movq $2, -312(%rbp)
movq -312(%rbp), %rdi

.L54:

# LoadIntConst(1, x67)
movq $1, -320(%rbp)
movq -320(%rbp), %rdi

# LoadIntConst(2, x68)
movq $2, -328(%rbp)
movq -328(%rbp), %rdi

# Call(<=, [x67, x68], x69)
xor %rax, %rax
movq -320(%rbp), %rdx
cmpq -328(%rbp), %rdx
setle %al
movq %rax, -336(%rbp)

# LoadIntConst(3, x70)
movq $3, -344(%rbp)
movq -344(%rbp), %rdi

# LoadIntConst(3, x71)
movq $3, -352(%rbp)
movq -352(%rbp), %rdi

# Call(<, [x70, x71], x72)
xor %rax, %rax
movq -344(%rbp), %rdx
cmpq -352(%rbp), %rdx
setl %al
movq %rax, -360(%rbp)

# Call(and, [x69, x72], x73)

cmpq $0, -368(%rbp)
jne .L64
jmp .L65

.L64:

# LoadIntConst(1, x74)
movq $1, -376(%rbp)
movq -376(%rbp), %rdi

jmp .L66

.L65:

# LoadIntConst(2, x75)
movq $2, -384(%rbp)
movq -384(%rbp), %rdi

.L66:

# LoadIntConst(1, x79)
movq $1, -392(%rbp)
movq -392(%rbp), %rdi

# LoadIntConst(2, x80)
movq $2, -400(%rbp)
movq -400(%rbp), %rdi

# Call(<=, [x79, x80], x81)
xor %rax, %rax
movq -392(%rbp), %rdx
cmpq -400(%rbp), %rdx
setle %al
movq %rax, -408(%rbp)

# LoadIntConst(3, x82)
movq $3, -416(%rbp)
movq -416(%rbp), %rdi

# LoadIntConst(3, x83)
movq $3, -424(%rbp)
movq -424(%rbp), %rdi

# Call(<, [x82, x83], x84)
xor %rax, %rax
movq -416(%rbp), %rdx
cmpq -424(%rbp), %rdx
setl %al
movq %rax, -432(%rbp)

# Call(or, [x81, x84], x85)

cmpq $0, -440(%rbp)
jne .L76
jmp .L77

.L76:

# LoadIntConst(1, x86)
movq $1, -448(%rbp)
movq -448(%rbp), %rdi

jmp .L78

.L77:

# LoadIntConst(2, x87)
movq $2, -456(%rbp)
movq -456(%rbp), %rdi

.L78:

# LoadIntConst(1, x91)
movq $1, -464(%rbp)
movq -464(%rbp), %rdi

# LoadIntConst(2, x92)
movq $2, -472(%rbp)
movq -472(%rbp), %rdi

# Call(==, [x91, x92], x93)
xor %rax, %rax
movq -464(%rbp), %rdx
cmpq -472(%rbp), %rdx
sete %al
movq %rax, -480(%rbp)

cmpq $0, -480(%rbp)
jne .L88
jmp .L89

.L88:

# LoadIntConst(3, x94)
movq $3, -488(%rbp)
movq -488(%rbp), %rdi

jmp .L90

.L89:

# LoadIntConst(4, x95)
movq $4, -496(%rbp)
movq -496(%rbp), %rdi

.L90:

# LoadIntConst(1, x99)
movq $1, -504(%rbp)
movq -504(%rbp), %rdi

# LoadIntConst(2, x100)
movq $2, -512(%rbp)
movq -512(%rbp), %rdi

# Call(!=, [x99, x100], x101)
xor %rax, %rax
movq -504(%rbp), %rdx
cmpq -512(%rbp), %rdx
setne %al
movq %rax, -520(%rbp)

cmpq $0, -520(%rbp)
jne .L96
jmp .L97

.L96:

# LoadIntConst(3, x102)
movq $3, -528(%rbp)
movq -528(%rbp), %rdi

jmp .L98

.L97:

# LoadIntConst(4, x103)
movq $4, -536(%rbp)
movq -536(%rbp), %rdi

.L98:

# LoadIntConst(0, x107)
movq $0, -544(%rbp)
movq -544(%rbp), %rdi

# LoadIntConst(1, x108)
movq $1, -552(%rbp)
movq -552(%rbp), %rdi

# Call(and, [x107, x108], x109)

# LoadIntConst(1, x110)
movq $1, -568(%rbp)
movq -568(%rbp), %rdi

# Call(or, [x109, x110], x111)

cmpq $0, -576(%rbp)
jne .L104
jmp .L105

.L104:

# LoadIntConst(1, x112)
movq $1, -584(%rbp)
movq -584(%rbp), %rdi

jmp .L106

.L105:

# LoadIntConst(2, x113)
movq $2, -592(%rbp)
movq -592(%rbp), %rdi

.L106:

# LoadIntConst(0, x114)
movq $0, -600(%rbp)
movq -600(%rbp), %rdi


# END

movq $0, %rax
movq %rbp, %rsp
popq %rbp
ret
