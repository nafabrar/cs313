.pos 0x10

irmovq start, %rdx        # %rdx = 0x1000
irmovq $0xbeef, %rsi      # %rsi = 0xbeef
rmmovq %rsi, 8(%rdx,8)    # store 0xbeef in (8 * 0x50 + 8)

.pos 0x50
.align 8
start:  .quad 0x0
        .quad 0x1
        .quad 0x2
end:    .quad 0x3

.pos 0x280
.align 8
.quad 0x0
.quad 0x0