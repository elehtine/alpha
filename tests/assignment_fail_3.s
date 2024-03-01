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

# LoadIntConst(42, x1)
movq $42, -8(%rbp)
movq -8(%rbp), %rsi

# LoadIntConst(13, x2)
movq $13, -16(%rbp)
movq -16(%rbp), %rsi

# LoadIntConst(0, x3)
movq $0, -24(%rbp)
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
