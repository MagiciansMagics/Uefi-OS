#include <interrupts/idt/idt.h>
#include <apic/pic.h>
#include <cpu/io.h>

IDT_data_t IDT[IDT_ENTRIES];
IDT_Ptr_data_t IDTR;

extern void* isr_stub_table[];

NORETURN
void
exception_handler(void)
{
    for (;;)
    {
        asm volatile ("hlt");
    }
}

void
idt_set_gate(int n, uint64_t handler, uint8_t flags)
{
    IDT[n].isr_low = (uint16_t)handler;
    IDT[n].kernel_cs = 0x08;
    IDT[n].ist = 0;
    IDT[n].reserved = 0;
    IDT[n].attributes = flags;
    IDT[n].isr_mid = (uint16_t)(handler >> 16);
    IDT[n].isr_high = (uint32_t)(handler >> 32);
}

void
init_idt(void)
{
    IDTR.base = (uint64_t)&IDT[0];

    IDTR.limit = (uint16_t)sizeof(IDT_data_t) * IDT_ENTRIES - 1;

    for (uint8_t vector = 0; vector < 32; vector++) 
    {
        idt_set_gate(vector, (uint64_t)isr_stub_table[vector], 0x8E);
    }

    asm volatile ("lidt %0" : : "m"(IDTR));

    remap_pic();

    outb(PIC1_DATA, 0b11111101);
    outb(PIC2_DATA, 0b11111111);

    asm volatile("sti");
}