#ifndef _KERNEL_IDT_H_
#define _KERNEL_IDT_H_

#pragma once

#include <types.h>

#define IDT_ENTRIES 256

typedef struct
{
    uint16_t isr_low;     // bits 0..15 of ISR address
    uint16_t kernel_cs;   // GDT selector
    uint8_t  ist : 3;     // Interrupt Stack Table offset (0-7)
    uint8_t  zero : 5;
    uint8_t  attributes;  // type and flags
    uint16_t isr_mid;     // bits 16..31 of ISR address
    uint32_t isr_high;    // bits 32..63 of ISR address
    uint32_t reserved;
} PACKED IDT_data_t;

typedef struct
{
    uint16_t limit;
    uint64_t base;
} PACKED IDT_Ptr_data_t;

NORETURN
void
exception_handler(void);

void
idt_set_gate(int n, uint64_t handler, uint8_t flags);

void
init_idt(void);

#endif