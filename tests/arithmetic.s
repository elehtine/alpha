.global main
.type main, @function
.extern printf
.extern scanf

.section .text
main:
pushq %rbp
movq %rsp, %rbp

# START


subq $88, %rsp
subq $8, %rsp

# LoadIntConst(1, x1)
movq $1, -8(%rbp)
movq -8(%rbp), %rsi

# LoadIntConst(1, x2)
movq $1, -16(%rbp)
movq -16(%rbp), %rsi

# LoadIntConst(3, x3)
movq $3, -24(%rbp)
movq -24(%rbp), %rsi

# Call(*, [x2, x3], x4)
movq -16(%rbp), %rax
imulq -24(%rbp), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rsi

# Call(+, [x1, x4], x5)
movq -8(%rbp), %rax
addq -32(%rbp), %rax
movq %rax, -40(%rbp)
movq -40(%rbp), %rsi

# LoadIntConst(1, x6)
movq $1, -48(%rbp)
movq -48(%rbp), %rsi

# Call(-, [x5, x6], x7)
movq -40(%rbp), %rax
subq -48(%rbp), %rax
movq %rax, -56(%rbp)
movq -56(%rbp), %rsi

# LoadIntConst(6, x8)
movq $6, -64(%rbp)
movq -64(%rbp), %rsi

# LoadIntConst(2, x9)
movq $2, -72(%rbp)
movq -72(%rbp), %rsi

# Call(/, [x8, x9], x10)
movq -64(%rbp), %rax
cqto
idivq -72(%rbp)
movq %rax, -80(%rbp)
movq -80(%rbp), %rsi

# Call(+, [x7, x10], x11)
movq -56(%rbp), %rax
addq -80(%rbp), %rax
movq %rax, -88(%rbp)
movq -88(%rbp), %rsi


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
