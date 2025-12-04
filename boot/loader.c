#include <loader.h>
#include <elf.h>

static EFI_STATUS
elf_load_kernel(uint8_t* buffer, uint64_t size, uint64_t* entry)
{
    EFI_STATUS status = EFI_SUCCESS;
    Elf64_Ehdr* header = (Elf64_Ehdr*)buffer;

    if (header->e_ident[EI_MAG0] != 0x7F ||
        header->e_ident[EI_MAG1] != 'E' ||
        header->e_ident[EI_MAG2] != 'L' ||
        header->e_ident[EI_MAG3] != 'F')
        return EFI_LOAD_ERROR;

    if (header->e_type != ET_EXEC)
        return EFI_LOAD_ERROR;

    Elf64_Phdr* ph = (Elf64_Phdr*)(buffer + header->e_phoff);

    uint64_t mem_start = UINT64_MAX;
    uint64_t mem_end   = 0;
    uint64_t max_align = 4096;

    /* ---- Calculate memory span ---- */
    for (uint32_t i = 0; i < header->e_phnum; i++, ph++)
    {
        if (ph->p_type != PT_LOAD) continue;

        if (ph->p_align > max_align)
            max_align = ph->p_align;

        uint64_t seg_begin = ph->p_vaddr;
        uint64_t seg_end   = ph->p_vaddr + ph->p_memsz;

        seg_begin &= ~(max_align - 1);
        seg_end    = (seg_end + max_align - 1) & ~(max_align - 1);

        if (seg_begin < mem_start) mem_start = seg_begin;
        if (seg_end   > mem_end)   mem_end   = seg_end;
    }

    uint64_t needed = mem_end - mem_start;

    void* program_memory_buffer = NULL;
    status = uefi_call_wrapper(
        BS->AllocatePool, 3, EfiLoaderData, needed, &program_memory_buffer
    );
    if (EFI_ERROR(status))
        return status;

    ph = (Elf64_Phdr*)(buffer + header->e_phoff);
    for (uint32_t i = 0; i < header->e_phnum; i++, ph++)
    {
        if (ph->p_type != PT_LOAD) continue;

        uint64_t rel = ph->p_vaddr - mem_start;
        uint8_t* dst = (uint8_t*)program_memory_buffer + rel;
        uint8_t* src = (uint8_t*)buffer + ph->p_offset;

        uefi_call_wrapper(CopyMem, 3, dst, src, ph->p_filesz);

        if (ph->p_memsz > ph->p_filesz)
        {
            uint64_t bss_size = ph->p_memsz - ph->p_filesz;
            uefi_call_wrapper(SetMem, 3, dst + ph->p_filesz, bss_size, 0);
        }
    }

    *entry = (uint64_t)((uintptr_t)program_memory_buffer + (header->e_entry - mem_start));

    return status;
}

static EFI_FILE_HANDLE
GetVolume(EFI_HANDLE image)
{
    EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
    EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
    EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
    EFI_FILE_HANDLE Volume;                                 /* the volume's interface */

    /* get the loaded image protocol interface for our "image" */
    uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
    /* get the volume handle */
    uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
    uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
    return Volume;
}

static UINT64
FileSize(EFI_FILE_HANDLE FileHandle)
{
    UINT64 ret;
    EFI_FILE_INFO       *FileInfo;         /* file information structure */
    /* get the file's size */
    FileInfo = LibFileInfo(FileHandle);
    ret = FileInfo->FileSize;
    FreePool(FileInfo);
    return ret;
}

EFI_STATUS
load_kernel(EFI_HANDLE ImageHandle, uint64_t* entry_addr, uint64_t* kernel_size)
{
    EFI_STATUS status = EFI_SUCCESS;
    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);

    CHAR16              *FileName = L"kernel.elf";
    EFI_FILE_HANDLE     FileHandle;

    status = uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

    if (EFI_ERROR(status))
    {
        Print(L"Failed to open file: %s\n", FileName);
        goto exit;
    }

    UINT64              ReadSize = FileSize(FileHandle);
    UINT8               *Buffer = AllocatePool(ReadSize);

    status = uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &ReadSize, Buffer);

    *kernel_size = ReadSize;

    if (EFI_ERROR(status))
    {
        Print(L"Failed to read the file!\n");
        goto read_failure;
    }

    status = elf_load_kernel(Buffer, ReadSize, entry_addr);

    if (EFI_ERROR(status))
    {
        Print(L"Failed to load kernel!\n");
        goto read_failure;
    }

    goto exit;

read_failure:
    uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
    FreePool(Buffer);
exit:
    return status;
}