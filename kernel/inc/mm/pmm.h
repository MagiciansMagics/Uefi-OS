#ifndef _KERNEL_PMM_H_
#define _KERNEL_PMM_H_

#pragma once

#include <types.h>
#include <boot.h>
#include <mm/allocator.h>

#define MAX_PMM_REGIONS 8
#define MAX_ALLOCS_STORED 32

typedef struct
{
    bool set;
    uint64_t base;
    uint8_t region;
} pmm_alloc_data_t;

typedef struct
{
    bool ready;
    allocator_data_t alloc;
} pmm_region_data_t;

typedef struct
{
    pmm_region_data_t regions[MAX_PMM_REGIONS];
    pmm_alloc_data_t allocs[MAX_ALLOCS_STORED];
} pmm_data_t;

pmm_data_t*
get_pmm(void);

uint64_t
pmm_alloc(uint64_t size);

bool
pmm_free(uint64_t addr);

void
init_pmm(memory_map_data_t* mm);

#endif