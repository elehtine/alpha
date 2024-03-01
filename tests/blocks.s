.global main
.type main, @function
.extern printf
.extern scanf

.section .text
main:
pushq %rbp
movq %rsp, %rbp

# START


subq $80, %rsp

# LoadIntConst(1, x1)
movq $1, -8(%rbp)
movq -8(%rbp), %rsi

# LoadIntConst(2, x2)
movq $2, -16(%rbp)
movq -16(%rbp), %rsi

# LoadIntConst(3, x3)
movq $3, -24(%rbp)
movq -24(%rbp), %rsi

# Call(+, [x2, x3], x4)
movq -16(%rbp), %rax
addq -24(%rbp), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rsi

# LoadIntConst(0, x5)
movq $0, -40(%rbp)
movq -40(%rbp), %rsi

# LoadIntConst(4, x6)
movq $4, -48(%rbp)
movq -48(%rbp), %rsi

# LoadIntConst(0, x7)
movq $0, -56(%rbp)
movq -56(%rbp), %rsi

# LoadIntConst(0, x8)
movq $0, -64(%rbp)
movq -64(%rbp), %rsi

# LoadIntConst(5, x9)
movq $5, -72(%rbp)
movq -72(%rbp), %rsi

# LoadIntConst(0, x10)
movq $0, -80(%rbp)
movq -80(%rbp), %rsi


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
