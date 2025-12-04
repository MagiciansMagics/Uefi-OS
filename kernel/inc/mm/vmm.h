#ifndef _KERNEL_VMM_H_
#define _KERNEL_VMM_H_

#pragma once

#include <types.h>
#include <boot.h>

typedef struct
{
    uint8_t Present : 1;
    uint8_t ReadWrite : 1;
    uint8_t UserSuper : 1;
    uint8_t WriteThrough : 1;
    uint8_t CacheDisabled : 1;
    uint8_t Accessed : 1;
    uint8_t Ignore0 : 1; 
    uint8_t LargerPages : 1;
    uint8_t Ignore1 : 1;

    uint8_t Available : 3;

    uint64_t Address : 52;
} PageDirEntry_data_t;

typedef struct
{ 
    PageDirEntry_data_t Entries[512];
} ALIGNED(0x1000) PageTable_data_t;

#define PAGE_PRESENT   (1ULL << 0)
#define PAGE_WRITABLE  (1ULL << 1)
#define PAGE_USER      (1ULL << 2)
#define PAGE_WRITE_TH  (1ULL << 3)
#define PAGE_CACHE_DIS (1ULL << 4)
#define PAGE_ACCESSED  (1ULL << 5)
#define PAGE_DIRTY     (1ULL << 6)
#define PAGE_HUGE      (1ULL << 7)
#define PAGE_GLOBAL    (1ULL << 8)

void
map_page(void* virtualAddress, void* physicalAddress);

void
init_vmm(boot_info_data_t* params);

#endif