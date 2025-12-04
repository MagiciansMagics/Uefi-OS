#ifndef _KERNEL_ALLOCATOR_H_
#define _KERNEL_ALLOCATOR_H_

#pragma once

#include <types.h>

#define MAX_ALLOCS 2048

/*

    Just a bumb allocator for now

*/

typedef struct allocator_block_data_t
{
    uint64_t base;
    uint64_t size;
    bool free;
} allocator_block_data_t;

typedef struct
{
    uint64_t base;
    uint64_t size;

    /*
    
        The alloc is for the start after metadata information.

    */

    uint64_t alloc_base;
    uint64_t alloc_size;

    allocator_block_data_t allocs[MAX_ALLOCS];
} allocator_data_t;

uint64_t
mem_alloc(allocator_data_t* alloc_info, uint64_t alloc_size);

bool
mem_free(allocator_data_t* alloc_info, uint64_t phys_start);

void
init_allocator(allocator_data_t* alloc_info, uint64_t base, uint64_t size);

#endif