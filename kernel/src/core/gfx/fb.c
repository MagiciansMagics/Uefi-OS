#include <gfx/fb.h>
#include <module/kmodule_manager.h>

void
init_fb(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* gop)
{
    uint64_t gop_ptr = (uint64_t)gop;
    install_global_variable("fb_info", &gop_ptr, sizeof(uint64_t));

    install_module("fb");
}