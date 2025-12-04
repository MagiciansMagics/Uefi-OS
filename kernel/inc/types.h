#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

#pragma once
// Standard libraries

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#define local static
#define flocal inline static

#define PACKED __attribute__((packed))
#define NORETURN __attribute__((noreturn))
#define ALIGNED(a) __attribute__((aligned(a)))

#define ALIGN_UP(x, a) (((x) + ((a)-1)) & ~((a)-1))

#endif