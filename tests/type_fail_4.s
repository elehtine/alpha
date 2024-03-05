.global main
.type main, @function
.extern printf
.extern scanf

.section .text
main:
pushq %rbp
movq %rsp, %rbp

# START


subq $32, %rsp

# LoadIntConst(1, x1)
movq $1, -8(%rbp)
movq -8(%rbp), %rsi

# LoadIntConst(13, x2)
movq $13, -16(%rbp)
movq -16(%rbp), %rsi

# Call(or, [x1, x2], x3)
movq -24(%rbp), %rsi

# LoadIntConst(0, x4)
movq $0, -32(%rbp)
movq -32(%rbp), %rsi


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
