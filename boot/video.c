#include <video.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL*
get_graphics(void)
{
    EFI_STATUS status = EFI_SUCCESS;
    EFI_GUID guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    status = uefi_call_wrapper(BS->LocateProtocol, 3, &guid, NULL, (void **)&gop);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to locate graphics output protocol: %d\n", status);
        return NULL;
    }

    return gop;
}