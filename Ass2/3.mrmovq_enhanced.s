.pos 0x100

irmovq start, %rdx      # %rdx = 0x200
mrmovq 8(%rdx,8), %rsi  # %rsi = 0xe

.align 8
.pos 0x200
start:  .quad 0x0
        .quad 0x1
        .quad 0x2
end:    .quad 0x3        

.align 8
.pos 0x1000
  .quad 0xb
  .quad 0xe