#ifndef _KERNEL_PRINT_H_
#define _KERNEL_PRINT_H_

#pragma once

#include <types.h>

#define FONT_HEIGHT 16
#define FONT_WIDTH 8

#define PRINTF_BUFFER_SIZE 256

typedef struct
{
    uint32_t x;
    uint32_t y;
} print_pos_data_t;

int
kprintf(const char* format, ...);

#endif