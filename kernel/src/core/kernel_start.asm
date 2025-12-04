[BITS 64]

[EXTERN kmain]
[GLOBAL _start]

_start:
    cld
    cli

    mov rsp, 0x70000

    jmp kmain
halt:
    hlt
    jmp halt

section .note.GNU-stack noalloc noexec nowrite progbits