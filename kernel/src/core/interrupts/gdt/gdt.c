#include <interrupts/gdt/gdt.h>

static GDTEntry_data_t gdt[5];
static GDTPtr_data_t gdt_ptr;

void
reload_segments(void)
{
    asm volatile(
        "mov $0x10, %%ax\n\t"   // Data segment selector (index 2)
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "pushq $0x08\n\t"       // Code segment selector (index 1)
        "lea 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"             // Far return to load CS
        "1:\n\t"
        :
        :
        : "rax"
    );
}

void
set_gdt_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[idx].limit_low    = limit & 0xFFFF;
    gdt[idx].base_low     = base & 0xFFFF;
    gdt[idx].base_middle  = (base >> 16) & 0xFF;
    gdt[idx].access       = access;
    gdt[idx].granularity  = (uint8_t)((limit >> 16) & 0x0F) | (flags << 4);
    gdt[idx].base_high    = (uint8_t)(base >> 24) & 0xFF;
}

void
init_gdt(void)
{
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint64_t)&gdt;

    // Null descriptor
    set_gdt_entry(0, 0, 0, 0, 0);

    // Kernel code segment (0x08 selector)
    set_gdt_entry(1, 0, 0, 0x9A, 0xA);  // Access: code segment, Flags: 64-bit, granularity

    // Kernel data segment (0x10 selector)
    set_gdt_entry(2, 0, 0, 0x92, 0xC);  // Access: data segment, Flags: 32-bit, granularity

    load_gdt(&gdt_ptr);
}