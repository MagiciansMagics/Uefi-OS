#include <interrupts/interrupts.h>
#include <interrupts/gdt/gdt.h>
#include <interrupts/idt/idt.h>

void
init_interrupts(void)
{
    init_gdt();

    init_idt();
}