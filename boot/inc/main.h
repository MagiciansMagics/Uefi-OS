#ifndef _BOOTLOADER_MAIN_H_
#define _BOOTLOADER_MAIN_H_

#include <efi.h>
#include <efilib.h>

#pragma once

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

#endif