#ifndef _BOOTLOADER_MEMORY_H_
#define _BOOTLOADER_MEMORY_H_

#include <efi.h>
#include <efilib.h>

#include <boot.h>

#pragma once

EFI_STATUS
get_memory_map(memory_map_data_t* mm);

#endif