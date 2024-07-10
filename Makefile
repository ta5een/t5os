NAME = t5os
ARCH = i686
BUILD_TYPE ?= DEBUG

SRCDIR = src
LIBDIR = src
OUTDIR = build
TOOLCHAIN_DIR = toolchain
# TODO: How to let `bear` know of this directory (for compile_commands.json)?
TOOLCHAIN_BIN_DIR = $(TOOLCHAIN_DIR)/local/$(ARCH)/bin

# CXX = $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-g++
# AS = $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-as
# LD = $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-g++
# NOTE: For now, make sure to add `TOOLCHAIN_BIN_DIR` to your `$PATH` so that
# `bear` can provide LSP support
CC = $(ARCH)-elf-gcc
AS = $(ARCH)-elf-as
LD = $(ARCH)-elf-gcc
QEMU ?= qemu-system-i386

CCFLAGS := -std=gnu23 -Wall -Wextra -I$(LIBDIR) -ffreestanding
ASFLAGS :=
LDFLAGS := -ffreestanding -lgcc -nostdlib
QEMUFLAGS := -serial stdio

C_SOURCES = $(shell find $(SRCDIR) -name '*.c')
ASM_SOURCES = $(shell find $(SRCDIR) -name '*.S')
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/%.o,$(C_SOURCES))
OBJECTS += $(patsubst $(SRCDIR)/%.S,$(OUTDIR)/%.S.o,$(ASM_SOURCES))

ifeq ($(ARCH),i686)
	ARCHDIR = $(SRCDIR)/kernel/arch/x86/32
else
$(error '$(ARCH)' is currently not supported)
endif

# Mimics CMake's `CMAKE_BUILD_TYPE`, as documented in this StackOverflow answer:
# https://stackoverflow.com/a/59314670/10052039
ifeq ($(BUILD_TYPE),DEBUG)
	# Produce debug info, no optimisation, define DEBUG=1
	CCFLAGS := -g -O0 -DDEBUG $(CCFLAGS)
	QEMUFLAGS += -s -S -d int,cpu_reset -no-shutdown -no-reboot
else ifeq ($(BUILD_TYPE),RELEASE)
	# Highest optimisation level
	CCFLAGS := -O3 $(CCFLAGS)
else
$(error Invalid BUILD_TYPE '$(BUILD_TYPE)')
endif

.PHONY: default help toolchain kernel iso qemu gdb clean

default: help
toolchain: $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-*
kernel: $(OUTDIR)/$(NAME).bin
iso: $(OUTDIR)/$(NAME).iso

# TODO: Print usage text -- for now just spit out variables
help:
	$(info --- Variables defined in this Makefile: ---)
	$(foreach v,$(.VARIABLES),\
		$(if $(filter file,$(origin $(v))),\
			$(info $(v) = "$($(v))")))
	$(info --- Variables defined from the command line: ---)
	$(foreach v,$(.VARIABLES),\
		$(if $(filter command line,$(origin $(v))),\
			$(info $(v) = "$($(v))")))
	@echo ""

$(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-*:
	@bash $(TOOLCHAIN_DIR)/build-cross-tools.sh

# FIXME: Adding `toolchain` as a prerequisite doesn't work as expected.
#
# When running `make` inside a running Docker container with a toolchain built
# for the host system, `make` attempts to rebuild the project. This fails when
# the host system uses a different binary format (e.g. Mach-O instead of ELF).
$(OUTDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CCFLAGS) -o $@ -c $<

$(OUTDIR)/%.S.o: $(SRCDIR)/%.S
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(OUTDIR)/$(NAME).bin: $(ARCHDIR)/linker.ld $(OBJECTS)
	$(LD) $(LDFLAGS) -T $< -o $@ $(OBJECTS)

$(OUTDIR)/$(NAME).iso: $(OUTDIR)/$(NAME).bin
	mkdir -p $(OUTDIR)/iso/boot/grub
	cp $< $(OUTDIR)/iso/boot
	echo 'set timeout=0' > $(OUTDIR)/iso/boot/grub/grub.cfg
	echo 'set default=0' >> $(OUTDIR)/iso/boot/grub/grub.cfg
	echo '' >> $(OUTDIR)/iso/boot/grub/grub.cfg
	echo "menuentry \"$(NAME)\" {" >> $(OUTDIR)/iso/boot/grub/grub.cfg
	echo "	multiboot /boot/$(NAME).bin" >> $(OUTDIR)/iso/boot/grub/grub.cfg
	echo '	boot' >> $(OUTDIR)/iso/boot/grub/grub.cfg
	echo '}' >> $(OUTDIR)/iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ $(OUTDIR)/iso
	rm -rf $(OUTDIR)/iso

qemu: $(OUTDIR)/$(NAME).iso
	$(QEMU) $(QEMUFLAGS) -cdrom $< -m 64

# NOTE: Ensure QEMU is already running in debug mode in a separate process
# TODO: Consider moving this into a shell script to better orchestrate this
gdb: $(OUTDIR)/$(NAME).bin
	gdb $<

clean:
	rm -rf $(OUTDIR)/*
