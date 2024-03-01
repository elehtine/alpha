.global main
.type main, @function
.extern printf
.extern scanf

.section .text
main:
pushq %rbp
movq %rsp, %rbp

# START


subq $104, %rsp
subq $8, %rsp

# LoadIntConst(1, x1)
movq $1, -8(%rbp)
movq -8(%rbp), %rsi

# LoadIntConst(2, x2)
movq $2, -16(%rbp)
movq -16(%rbp), %rsi

# Call(*, [x2, x_1], x3)
movq -16(%rbp), %rax
imulq -24(%rbp), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rsi

# Call(+, [x_1, x_2], x4)
movq -24(%rbp), %rax
addq -40(%rbp), %rax
movq %rax, -48(%rbp)
movq -48(%rbp), %rsi

# Call(-, [x_2, x_1], x5)
movq -40(%rbp), %rax
subq -24(%rbp), %rax
movq %rax, -56(%rbp)
movq -56(%rbp), %rsi

# LoadIntConst(3, x6)
movq $3, -64(%rbp)
movq -64(%rbp), %rsi

# LoadIntConst(4, x7)
movq $4, -72(%rbp)
movq -72(%rbp), %rsi

# LoadIntConst(0, x8)
movq $0, -80(%rbp)
movq -80(%rbp), %rsi

# LoadIntConst(1337, x9)
movq $1337, -88(%rbp)
movq -88(%rbp), %rsi

# LoadIntConst(42, x10)
movq $42, -96(%rbp)
movq -96(%rbp), %rsi

# LoadIntConst(0, x11)
movq $0, -104(%rbp)
movq -104(%rbp), %rsi


# END
movq $print_format, %rdi
call printf

.Lend:
movq %rbp, %rsp
popq %rbp
ret

scan_format:
.asciz "%ld"
print_format:
.asciz "%ld\n"
