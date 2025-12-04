[BITS 64]
[GLOBAL load_gdt]

[EXTERN reload_segments]

load_gdt:
    mov rax, [rdi]          ; load limit
    lgdt [rdi]              ; load GDT from pointer
    call reload_segments    ; call a function to reload segment selectors
    ret