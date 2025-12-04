#ifndef _BOOT_H_
#define _BOOT_H_

#include <efi.h>

typedef struct
{
    UINTN mm_size;
    EFI_MEMORY_DESCRIPTOR* mm_descriptor;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
} memory_map_data_t;

typedef struct
{
    memory_map_data_t mm;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE gop;

    UINT64 kernel_start;
    UINT64 kernel_size;

    void* rsdp_ptr;
} boot_info_data_t;

typedef void (*kernel_entry)(boot_info_data_t *params);

#endif