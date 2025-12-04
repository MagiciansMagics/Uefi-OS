#ifndef _KERNEL_CPU_UTILS_H_
#define _KERNEL_CPU_UTILS_H_

#pragma once

#include <types.h>

flocal void
cr3_write(uint64_t value)
{
    asm volatile("mov %0, %%cr3" : : "r"(value));
}

#endif