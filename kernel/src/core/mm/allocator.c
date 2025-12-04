#include <mm/allocator.h>
#include <string.h>
#include <gfx/print.h>

#define PAGE_SIZE 4096
#define ALIGN_PAGE(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

uint64_t
mem_alloc(allocator_data_t* alloc_info, uint64_t alloc_size)
{
    if (!alloc_info || !alloc_size || !alloc_info->alloc_size)
        return 0;

    alloc_size = ALIGN_PAGE(alloc_size);

    uint64_t a = 0;

    for (uint16_t i = 0; i < MAX_ALLOCS; i++)
    {
        if (!alloc_info->allocs[i].free) {
            a += alloc_info->allocs[i].size;
            continue;
        }

        uint64_t base = alloc_info->alloc_base + a;

        uint64_t aligned_base = ALIGN_PAGE(base);

        uint64_t align_gap = aligned_base - base;

        if (a + align_gap + alloc_size > alloc_info->alloc_size)
            return 0;

        alloc_info->allocs[i].base = aligned_base;
        alloc_info->allocs[i].size = alloc_size + align_gap; // reserve the gap too
        alloc_info->allocs[i].free = false;

        return aligned_base;
    }

    return 0;
}

/*

    TODO:

    1. Freed memory can't be reused in the middle

    Example:

    Used: [4 KB][4 KB][FREE][4 KB]

*/

bool
mem_free(allocator_data_t* alloc_info, uint64_t phys_start)
{
    if (alloc_info == NULL)
        return 0;

    if (phys_start < alloc_info->alloc_base || phys_start >= alloc_info->alloc_base + alloc_info->alloc_size)
        return false;

    for (uint16_t i = 0; i < MAX_ALLOCS; i++)
    {
        if (alloc_info->allocs[i].base == phys_start && !alloc_info->allocs[i].free)
        {
            if (alloc_info->allocs[i].size == 0)
                return false;

            if (phys_start + alloc_info->allocs[i].size > alloc_info->alloc_base + alloc_info->alloc_size)
                return false;

            memset((void*)alloc_info->allocs[i].base, 0, alloc_info->allocs[i].size);

            alloc_info->allocs[i].free = true;
            alloc_info->allocs[i].size = 0;
            alloc_info->allocs[i].base = 0;

            return true;
        }
    }

    return false;
}

void
init_allocator(allocator_data_t* alloc_info, uint64_t base, uint64_t size)
{
    memset(alloc_info, 0, sizeof(allocator_data_t));

    alloc_info->base = ALIGN_PAGE(base);
    alloc_info->size = size & ~(PAGE_SIZE - 1);

    alloc_info->alloc_base = alloc_info->base;
    alloc_info->alloc_size = alloc_info->size;

    for (uint16_t i = 0; i < MAX_ALLOCS; i++)
    {
        alloc_info->allocs[i].free = true;
    }
}