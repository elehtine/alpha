.global main
.type main, @function
.extern printf
.extern scanf

.section .text
main:
pushq %rbp
movq %rsp, %rbp

# START


subq $24, %rsp
subq $8, %rsp

# LoadIntConst(1, x1)
movq $1, -8(%rbp)
movq -8(%rbp), %rsi

# LoadIntConst(2, x2)
movq $2, -16(%rbp)
movq -16(%rbp), %rsi

# Call(+, [x1, x2], x3)
movq -8(%rbp), %rax
addq -16(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rsi


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
