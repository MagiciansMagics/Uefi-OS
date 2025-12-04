#include <main.h>
#include <memory.h>
#include <video.h>
#include <loader.h>

#include <string.h>

static EFI_GUID acpi20_guid = ACPI_20_TABLE_GUID;
static EFI_GUID acpi_guid   = ACPI_TABLE_GUID;

static void
halt_system(void)
{
    while (1)
    {
        asm volatile ("hlt");
    }
}

static UINT8
compare(const void* firstitem, const void* seconditem, UINT64 comparelength)
{
    // Using const since this is a read-only operation: absolutely nothing should be changed here.
    const UINT8 *one = firstitem, *two = seconditem;
    for (UINT64 i = 0; i < comparelength; i++)
    {
        if(one[i] != two[i])
        {
            return 0;
        }
    }
    return 1;
}

static void*
get_acpi_rsdp(void)
{
    void* rsdp_ptr = NULL;

    int version = 0;

    for(UINTN i = 0; i < ST->NumberOfTableEntries; i++)
    {
        if (compare(&ST->ConfigurationTable[i].VendorGuid, &acpi20_guid, 16))
        {
            rsdp_ptr = ST->ConfigurationTable[i].VendorTable;
            version = 1;
        }
    }

    if (!version)
    {
        for (UINT8 i = 0; i < ST->NumberOfTableEntries; i++)
        {
            if (compare(&ST->ConfigurationTable[i].VendorGuid, &acpi_guid, 16))
            {
                rsdp_ptr = ST->ConfigurationTable[i].VendorTable;
            }
        }
    }

    return rsdp_ptr;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS status = EFI_SUCCESS;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
    boot_info_data_t boot_params = {0};

    InitializeLib(ImageHandle, SystemTable);

    // Clear the screen!
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    status = BS->SetWatchdogTimer (0, 0, 0, NULL);
    if(EFI_ERROR(status))
    {
        Print(L"Error stopping watchdog\n");
    }

    Print(L"Firmware Vendor: %s\nFirmware Revision: 0x%08x\n", ST->FirmwareVendor, ST->FirmwareRevision);

    gop = get_graphics();
    boot_params.gop = *gop->Mode;

    uint64_t entry_addr = 0;
    uint64_t kernel_size = 0;
    status = load_kernel(ImageHandle, &entry_addr, &kernel_size);

    if (EFI_ERROR(status))
    {
        Print(L"Failed to load the kernel!\n");
        goto halt;
    }

    boot_params.rsdp_ptr = get_acpi_rsdp();
    boot_params.kernel_start = entry_addr;
    boot_params.kernel_size = kernel_size;

    Print(L"Kernel entry: 0x%lx, kernel size: 0x%lx\n", entry_addr, kernel_size);

    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    status = get_memory_map(&boot_params.mm);

    if (EFI_ERROR(status))
    {
        Print(L"Getting memory map failed!\n");
        goto halt;
    }

    // Exit boot services!
    status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, boot_params.mm.map_key);

    if (EFI_ERROR(status))
    {
        Print(L"Failed to exit boot services: %d\n", status);
        goto halt;
    }

    ((kernel_entry)entry_addr)(&boot_params);
    // Halt for now because we're testing!
halt:
    halt_system();

    return status;
}