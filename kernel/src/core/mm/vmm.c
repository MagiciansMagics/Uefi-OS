#include <mm/vmm.h>
#include <mm/pmm.h>
#include <cpu/utils.h>
#include <gfx/print.h>
#include <string.h>

extern void high_entry(uint64_t stack_addr);

// "pmm_alloc" aligns always to first 4096 size! It also return a physical address!

PageTable_data_t* PML4;

void
map_page(void* virtualAddress, void* physicalAddress)
{
    uint64_t indexer = (uint64_t)virtualAddress;
    indexer >>= 12;
    uint64_t pIndex = indexer & 0x1ff;

    indexer >>= 9;
    uint64_t ptIndex = indexer & 0x1ff;

    indexer >>= 9;
    uint64_t pdIndex = indexer & 0x1ff;

    indexer >>= 9;
    uint64_t pdpIndex = indexer & 0x1ff;

    PageDirEntry_data_t pde;

    pde = PML4->Entries[pdpIndex];
    PageTable_data_t* pdp;
    if (!pde.Present)
    {
        pdp = (PageTable_data_t*)pmm_alloc(1);
        memset(pdp, 0, 0x1000);
        pde.Address = (uint64_t)pdp >> 12;
        pde.Present = 1;
        pde.ReadWrite = 1;
        PML4->Entries[pdpIndex] = pde;
    }
    else
    {
        pdp = (PageTable_data_t*)((uint64_t)pde.Address << 12);
    }
    
    
    pde = pdp->Entries[pdIndex];
    PageTable_data_t* pd;
    if (!pde.Present)
    {
        pd = (PageTable_data_t*)pmm_alloc(1);
        memset(pd, 0, 0x1000);
        pde.Address = (uint64_t)pd >> 12;
        pde.Present = 1;
        pde.ReadWrite = 1;
        pdp->Entries[pdIndex] = pde;
    }
    else
    {
        pd = (PageTable_data_t*)((uint64_t)pde.Address << 12);
    }

    pde = pd->Entries[ptIndex];
    PageTable_data_t* pt;
    if (!pde.Present)
    {
        pt = (PageTable_data_t*)pmm_alloc(1);
        memset(pt, 0, 0x1000);
        pde.Address = (uint64_t)pt >> 12;
        pde.Present = 1;
        pde.ReadWrite = 1;
        pd->Entries[ptIndex] = pde;
    }
    else
    {
        pt = (PageTable_data_t*)((uint64_t)pde.Address << 12);
    }

    pde = pt->Entries[pIndex];
    pde.Address = (uint64_t)physicalAddress >> 12;
    pde.Present = 1;
    pde.ReadWrite = 1;
    pt->Entries[pIndex] = pde;
}

void
map_region(void* virt, void* phys, uint64_t size)
{
    for (uint64_t i = 0; i < size; i += 0x1000)
    {
        map_page(virt + i, phys + i);
    }
}

void
init_vmm(boot_info_data_t* params)
{
    uint64_t pml4_phys = pmm_alloc(1);
    PML4 = (PageTable_data_t*)pml4_phys;
    memset(PML4, 0, 4096);

    uint64_t kernel_start = params->kernel_start;
    uint64_t kernel_size = params->kernel_size;

    // Kernel
    map_region((void*)kernel_start, (void*)kernel_start, kernel_size);

    // Pmm regions
    pmm_data_t* pmm = get_pmm();
    for (uint64_t i = 0; i < MAX_PMM_REGIONS; i++)
    {
        map_region((void*)pmm->regions[i].alloc.base, (void*)pmm->regions[i].alloc.base, pmm->regions[i].alloc.size);
    }

    uint64_t stack_size = 0x8000;
    uint64_t stack_phys = pmm_alloc(stack_size);

    map_region((void*)stack_phys, (void*)stack_phys, stack_size);
    __asm__ volatile (
        "mov %0, %%rsp\n"  // Set RSP to the top of the stack
        "mov %1, %%cr3\n"  // Set CR3 to the physical address of PML4
        :
        : "r"(stack_phys + stack_size), "r"(pml4_phys)
        : "memory"
    );
}