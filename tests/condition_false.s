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

# LoadIntConst(0, x3)
movq $0, -8(%rbp)
movq -8(%rbp), %rsi

.L1

# LoadIntConst(1, x4)
movq $1, -16(%rbp)
movq -16(%rbp), %rsi

.L2

# LoadIntConst(0, x5)
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
