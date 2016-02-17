.pos 0x100

irmovq start, %rdx    # %rdx = 0x200
irmovq $0xbeef, %rsi  # %rsi = 0xbeef
rmmovq %rsi, 8(%rdx)  # M[0x208] = 0xbeef

.align 8
.pos 0x200
start:  .quad 0x0
        .quad 0x1
        .quad 0x2
end:    .quad 0x3        