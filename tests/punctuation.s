.global main
.type main, @function
.extern printf
.extern scanf

.section .text
main:
pushq %rbp
movq %rsp, %rbp

# START


subq $40, %rsp
subq $8, %rsp

# LoadIntConst(3, x1)
movq $3, -8(%rbp)
movq -8(%rbp), %rsi

# LoadIntConst(2, x2)
movq $2, -16(%rbp)
movq -16(%rbp), %rsi

# LoadIntConst(1, x3)
movq $1, -24(%rbp)
movq -24(%rbp), %rsi

# Call(+, [x2, x3], x4)
movq -16(%rbp), %rax
addq -24(%rbp), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rsi

# Call(*, [x1, x4], x5)
movq -8(%rbp), %rax
imulq -32(%rbp), %rax
movq %rax, -40(%rbp)
movq -40(%rbp), %rsi


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
