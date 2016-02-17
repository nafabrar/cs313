.pos 0x100

irmovq start, %rdx    # %rdx = 0x200
mrmovq 8(%rdx), %rsi  # %rsi = 0x1

.align 8
.pos 0x200
start:  .quad 0x0
        .quad 0x1
        .quad 0x2
end:    .quad 0x3        