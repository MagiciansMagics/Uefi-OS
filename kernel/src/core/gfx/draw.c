#include <gfx/draw.h>
#include <module/kmodule_manager.h>

void
draw_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (!require_module("fb"))
        return;

    uint64_t fb_ptr = 0;

    if (!require_global_variable("fb_info", &fb_ptr))
        return;

    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* gop = (EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*)fb_ptr;

    uint32_t* framebuffer = (uint32_t*)gop->FrameBufferBase;
    uint32_t width = gop->Info->HorizontalResolution;
    uint32_t height = gop->Info->VerticalResolution;

    framebuffer[y * width + x] = color;
}