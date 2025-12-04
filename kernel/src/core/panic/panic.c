#include <panic/panic.h>

NORETURN
void
kpanic(void)
{
    for (;;)
    {
        asm volatile ("hlt");
    }
}