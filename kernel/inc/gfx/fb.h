#ifndef _KERNEL_FB_H
#define _KERNEL_FB_H

#pragma once

#include <types.h>
#include <boot.h>

void
init_fb(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* gop);

#endif