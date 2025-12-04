#include <memory.h>

EFI_STATUS
get_memory_map(memory_map_data_t* mm)
{
    EFI_STATUS status = EFI_SUCCESS;

    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &mm->mm_size, mm->mm_descriptor, &mm->map_key, &mm->descriptor_size, &mm->descriptor_version);
    
    if (EFI_ERROR(status) && status != EFI_BUFFER_TOO_SMALL)
    {
        return status;
    }

    mm->mm_size += mm->descriptor_size * 2;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, mm->mm_size, &mm->mm_descriptor);
    if (EFI_ERROR(status))
    {
        return status;
    }

    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &mm->mm_size, mm->mm_descriptor, &mm->map_key, &mm->descriptor_version, &mm->descriptor_size);
    if (EFI_ERROR(status))
    {
        uefi_call_wrapper(BS->FreePool, 1, mm->mm_descriptor);
    }

    
    return status;
}