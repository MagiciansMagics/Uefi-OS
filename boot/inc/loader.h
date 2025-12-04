#ifndef _BOOTLOADER_LOADER_H_
#define _BOOTLOADER_LOADER_H_

#include <efi.h>
#include <efilib.h>

#pragma once

EFI_STATUS
load_kernel(EFI_HANDLE ImageHandle, uint64_t* entry_addr, uint64_t* kernel_size);

#endif