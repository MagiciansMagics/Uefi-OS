#include <kernel.h>
#include <module/kmodule_manager.h>
#include <gfx/fb.h>
#include <gfx/print.h>
#include <interrupts/interrupts.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <panic/panic.h>

NORETURN
void
kmain(boot_info_data_t* params)
{
    init_kmodule_manager();

    init_fb(&params->gop);

    init_interrupts();

    kprintf("Welcome to my operating system\n");

    init_pmm(&params->mm);

    kprintf("Kernel addr: 0x%x, kernel size: 0x%x\n", params->kernel_start, params->kernel_size);

    init_vmm(params);

    kpanic();
}