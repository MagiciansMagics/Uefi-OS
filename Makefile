# ------------------------------
# FUNCTIONS
# ------------------------------

GCC								:=							gcc
NASM							:=							nasm -f elf64
LD								:=							ld
OBJCOPY							:=							objcopy
EMULATOR						:=							qemu-system-x86_64

# ------------------------------
# FLAGS
# ------------------------------

ARCH							:=							x86_64

GCC_FLAGS_BOOTLOADER			:=							-m64 -ffreestanding -c -fpic -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -I boot/inc/
GCC_FLAGS_KERNEL				:=							-m64 -ffreestanding -c -I kernel/inc/ -I boot/inc/ -I kernel/src/libc/inc/ -g
LD_FLAGS_BOOTLOADER				:=							-shared -Bsymbolic
LD_FLAGS_KERNEL					:= 

OBJCOPY_FLAGS					:=							-j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10

# ------------------------------
# LIBRARIES
# ------------------------------

EFI_LIB							:=							gnu-efi/inc/

# ------------------------------
# LINKER FILES
# ------------------------------

LD_LINKER_FILE					:=							gnu-efi/gnuefi/elf_x86_64_efi.lds
LD_STARTUP_FILE					:=							gnu-efi/$(ARCH)/gnuefi/crt0-efi-x86_64.o

LIBEFI 							:=							gnu-efi/$(ARCH)/lib/libefi.a
LIBGNUEFI						:=							gnu-efi/$(ARCH)/gnuefi/libgnuefi.a

# ------------------------------
# FILES
# ------------------------------

BOOTLOADER_FILE					:=							boot.efi
KERNEL_FILE						:=							kernel.elf

OS_IMAGE						:=							os.img

# ------------------------------
# FOLDER PATHS
# ------------------------------

BOOT_SRC_DIR					:=							boot
BOOT_OBJ_DIR					:=							build

KERNEL_SRC_DIR					:=							kernel/src
KERNEL_OBJ_DIR					:=							build2

# ------------------------------
# SOURCES
# ------------------------------

BOOT_C_FILES					:=							$(shell find $(BOOT_SRC_DIR) -name "*.c")
BOOT_O_FILES					:=							$(BOOT_C_FILES:$(BOOT_SRC_DIR)/%.c=$(BOOT_OBJ_DIR)/%.o)

KERNEL_C_FILES					:=							$(shell find $(KERNEL_SRC_DIR) -type f -name "*.c")
KERNEL_O_FILES					:=							$(KERNEL_C_FILES:$(KERNEL_SRC_DIR)/%.c=$(KERNEL_OBJ_DIR)/%.o)

# ------------------------------
# TARGET
# ------------------------------

all: setup create_img create_font $(BOOTLOADER_FILE) $(KERNEL_FILE) create_os_img clean

# ------------------------------
# COMPILE
# ------------------------------

# BOOTLOADER

$(BOOT_OBJ_DIR)/%.o: $(BOOT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(GCC) $(GCC_FLAGS_BOOTLOADER) -I $(EFI_LIB) -o $@ $<

$(BOOTLOADER_FILE): $(BOOT_O_FILES)
	@$(LD) $(LD_FLAGS_BOOTLOADER) -T $(LD_LINKER_FILE) $(LD_STARTUP_FILE) $(BOOT_O_FILES) $(LIBEFI) $(LIBGNUEFI) -o $(BOOT_OBJ_DIR)/$(BOOTLOADER_FILE)
	@$(OBJCOPY) $(OBJCOPY_FLAGS) $(BOOT_OBJ_DIR)/$(BOOTLOADER_FILE) $(BOOT_OBJ_DIR)/$(BOOTLOADER_FILE)

# KERNEL

$(KERNEL_OBJ_DIR)/%.o: $(KERNEL_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(GCC) $(GCC_FLAGS_KERNEL) -I $(EFI_LIB) -o $@ $<

$(KERNEL_FILE): $(KERNEL_O_FILES)
	@$(NASM) kernel/src/core/kernel_start.asm -o $(KERNEL_OBJ_DIR)/kernel_start.o
	@$(NASM) kernel/src/core/interrupts/gdt/gdt_load.asm -o $(KERNEL_OBJ_DIR)/gdt_load.o
	@$(NASM) kernel/src/core/interrupts/idt/idt.asm -o $(KERNEL_OBJ_DIR)/idt_table.o
	@$(LD) -T kernel/src/kernel.ld -o $(KERNEL_OBJ_DIR)/$(KERNEL_FILE) \
		$(KERNEL_OBJ_DIR)/kernel_start.o \
		$(KERNEL_O_FILES) \
		$(KERNEL_OBJ_DIR)/gdt_load.o \
		$(KERNEL_OBJ_DIR)/idt_table.o \
		$(KERNEL_OBJ_DIR)/font.o

# ------------------------------
# IMAGE BUILDING
# ------------------------------

create_img:
	@bash tools/create_img.sh

create_font:
	@python3 tools/font.py fonts/ter-116n.psf $(KERNEL_OBJ_DIR)/font.raw

	@objcopy -I binary -O elf64-x86-64 \
		$(KERNEL_OBJ_DIR)/font.raw $(KERNEL_OBJ_DIR)/font.o

create_os_img:
	@sudo losetup --offset 1048576 --sizelimit 46934528 /dev/loop99 $(OS_IMAGE)
	@sudo mount /dev/loop99 /mnt
	@sudo mkdir -p /mnt/EFI/BOOT/
	@sudo cp $(BOOT_OBJ_DIR)/$(BOOTLOADER_FILE) /mnt/EFI/BOOT/BOOTX64.EFI
	@sudo cp $(KERNEL_OBJ_DIR)/$(KERNEL_FILE) /mnt
	@sudo umount /mnt
	@sudo losetup -d /dev/loop99

	@cp $(OS_IMAGE) /home/magician/Downloads

setup:
	@mkdir -p $(BOOT_OBJ_DIR)
	@mkdir -p $(KERNEL_OBJ_DIR)

clean:
	@rm -rf $(BOOT_OBJ_DIR)
	@rm -rf $(KERNEL_OBJ_DIR)

run: all
	$(EMULATOR) \
	-bios /usr/share/ovmf/OVMF.fd \
    -drive file=$(OS_IMAGE),format=raw \
    -d int,mmu \
    -D qemu.log