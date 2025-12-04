#ifndef _KERNEL_DRAW_H_
#define _KERNEL_DRAW_H_

#pragma once

#include <types.h>
#include <boot.h>

void
draw_pixel(uint32_t x, uint32_t y, uint32_t color);

#endif