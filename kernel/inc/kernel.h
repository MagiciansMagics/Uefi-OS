#ifndef _KERNEL_MAIN_H_
#define _KERNEL_MAIN_H_

#include <boot.h>
#include <stdint.h>

void
kmain(boot_info_data_t* params);

void
kmain_high(void);

#endif