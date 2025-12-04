#include <mm/pmm.h>
#include <module/kmodule_manager.h>
#include <gfx/print.h>
#include <string.h>

pmm_data_t pmm = {0};

pmm_data_t*
get_pmm(void)
{
    return &pmm;
}

uint64_t
pmm_alloc(uint64_t size)
{
    for (uint64_t region = 0; region < MAX_PMM_REGIONS; region++)
    {
        if (!pmm.regions[region].ready)
            continue;

        uint64_t base = mem_alloc(&pmm.regions[region].alloc, size);
        if (!base)
            continue;

        for (uint64_t i = 0; i < MAX_ALLOCS_STORED; i++)
        {
            if (!pmm.allocs[i].set)
            {
                pmm.allocs[i].region = region;
                pmm.allocs[i].base   = base;
                pmm.allocs[i].set    = true;
                kprintf("[PMM] Alloc at: 0x%x, region: %d\n", base, region);
                return base;
            }
        }

        mem_free(&pmm.regions[region].alloc, base);
        return 0;
    }

    return 0;
}

bool
pmm_free(uint64_t addr)
{
    for (uint64_t i = 0; i < MAX_ALLOCS_STORED; i++)
    {
        if (!pmm.allocs[i].set || pmm.allocs[i].base != addr)
            continue;

        uint8_t region = pmm.allocs[i].region;

        if (!pmm.regions[region].ready)
            return false;

        if (mem_free(&pmm.regions[region].alloc, addr))
        {
            pmm.allocs[i].set = false;
            return true;
        }
    }

    return false;
}

void
init_pmm(memory_map_data_t* mmap)
{
    for (uint64_t i = 0; i < MAX_PMM_REGIONS; i++)
        memset(&pmm.regions[i], 0, sizeof(pmm_region_data_t));

    for (uint64_t i = 0; i < MAX_ALLOCS_STORED; i++)
        memset(&pmm.allocs[i], 0, sizeof(pmm_alloc_data_t));

    uint64_t mmap_entries = mmap->mm_size / mmap->descriptor_size;

    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)mmap->mm_descriptor;

    uint64_t region_count = 0;

    for (uint64_t i = 0; i < mmap_entries; i++)
    {
        if (desc->Type == EfiConventionalMemory)
        {
            if (region_count >= MAX_PMM_REGIONS)
                break;
            uint64_t region_size = desc->NumberOfPages * 4096;
            uint64_t region_base = desc->PhysicalStart;

            if (!region_size)
                continue;

            pmm.regions[region_count].ready = true;
            init_allocator(&pmm.regions[region_count].alloc, region_base, region_size);

            region_count++;

            kprintf("Initialized a PMM region:\n");
            kprintf("   Region id: %d\n", region_count);
            kprintf("   Region base: 0x%x\n", region_base);
            kprintf("   Region size: 0x%x\n\n", region_size);
        }

        desc = (EFI_MEMORY_DESCRIPTOR*)((uint8_t*)desc + mmap->descriptor_size);
    }

    install_module("pmm");
}