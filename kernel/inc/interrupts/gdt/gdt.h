#ifndef _KERNEL_GDT_H_
#define _KERNEL_GDT_H_

#pragma once

#include <types.h>

typedef struct 
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} PACKED GDTEntry_data_t;

typedef struct
{
    uint16_t limit;
    uint64_t base;
} PACKED GDTPtr_data_t;

// Defined in gdt_load.asm!
extern void
load_gdt(GDTPtr_data_t* gdt_ptr);

void
reload_segments(void);

void
set_gdt_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

void
init_gdt(void);

#endif