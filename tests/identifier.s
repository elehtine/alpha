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

# Call(+, [abc, a1], x1)
movq $1, -8(%rbp)
movq $1, -16(%rbp)
movq -8(%rbp), %rax
addq -16(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rsi

# LoadIntConst(5, x2)
movq $5, -32(%rbp)
movq -32(%rbp), %rsi

# Call(+, [x1, x2], x3)
movq -24(%rbp), %rax
addq -32(%rbp), %rax
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
