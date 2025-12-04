#ifndef _BOOTLOADER_VIDEO_H_
#define _BOOTLOADER_VIDEO_H_

#include <efi.h>
#include <efilib.h>

#pragma once

EFI_GRAPHICS_OUTPUT_PROTOCOL*
get_graphics(void);

#endif