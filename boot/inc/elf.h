#ifndef _BOOTLOADER_ELF_H_
#define _BOOTLOADER_ELF_H_

#include <stdint.h>
#include <stdbool.h>

#define EI_NIDENT 16

// e_type
#define ET_NONE         0               // No file type
#define ET_REL          1               // Relocatable file
#define ET_EXEC         2               // Executable file
#define ET_DYN          3               // Shared object file
#define ET_CORE         4               // Core file
#define ET_LOPROC       0xFF00          // Processor specific. Wth?
#define ET_HIPROC       0xFFFF          // Processor specific. Also wth?

// e_machine. The member's value specifies the required architecture for an invidual file
#define EM_NONE         0               // No machine
#define EM_M32          1               // AT&T WE 321000
#define EM_SPARC        2               // SPARC
#define EM_386          3               // Intel 80386
#define EM68K           4               // Motorola 68000
#define EM_88K          5               // Motorola 88000
#define EM_860          7               // Intel 80860
#define EM_MIPS         8               // MIPS RS3000

// EI_DATA
#define ELFDATANONE     0               // Invalid data encoding
#define ELFDATA2LSB     1               // ???
#define ELFDATA2MSB     2               // ???

// e_version.
#define EV_NONE         0               // Invalid version
#define EV_CURRENT      1               // Current version

// Elf identification
#define EI_MAG0         0               // File identification
#define EI_MAG1         1               // File identification
#define EI_MAG2         2               // File identification
#define EI_MAG3         3               // File identification
#define EI_CLASS        4               // File class
#define EI_DATA         5               // Data encoding
#define EI_VERSION      6               // File version
#define EI_PAD          7               // Start of padding bytes
#define EI_NIDENT       16              // Size of e_ident[]

// Elf architecture
#define ELFCLASSNONE    0               // Invalid class
#define ELFCLASS32      1               // 32-Bit objects
#define ELFCLASS64      2               // 64-Bit onjects

// sh_type
#define SHT_NULL        0               // Header is inactive
#define SHT_PROGBITS    1               // Holds information defined by the program, whose format and meaning a detemined solely by the program
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11

// Phdr types
#define PT_NULL         0
#define PT_LOAD         1

// Types

typedef uint64_t  Elf64_Addr;
typedef uint64_t  Elf64_Off;

typedef uint16_t  Elf64_Half;
typedef uint32_t  Elf64_Word;
typedef int32_t   Elf64_Sword;

typedef uint64_t  Elf64_Xword;
typedef int64_t   Elf64_Sxword;

typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef struct
{
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

#endif