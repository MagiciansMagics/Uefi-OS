#ifndef _KERNEL_PANIC_H_
#define _KERNEL_PANIC_H_

#pragma once

#include <types.h>

NORETURN
void
kpanic(void);

#endif