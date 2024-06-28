NAME = wyoos
ARCH = i686
DEBUG ?= 1

SRCDIR = src
LIBDIR = src
OUTDIR = out
# TODO: This is hard-coded for now
ARCHDIR = $(SRCDIR)/kernel/platform/x86/i386
# TODO: How to let `bear` know of this directory (for compile_commands.json)?
TOOLCHAIN_BIN_DIR = toolchain/local/$(ARCH)/bin

CXX = $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-g++
AS = $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-as
LD = $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-g++
QEMU ?= qemu-system-i386

CXXFLAGS := -std=c++20 -Wall -Wextra -I$(LIBDIR) -ffreestanding -fno-exceptions -fno-rtti -fno-use-cxa-atexit
ASFLAGS :=
LDFLAGS := -ffreestanding -lgcc -nostdlib
QEMUFLAGS :=

ifeq ($(DEBUG),1)
	# Prepend debug flags to make it more prominent
	CXXFLAGS := -g -DDEBUG_KERNEL $(CXXFLAGS)
	QEMUFLAGS += -s -S -d int,cpu_reset
endif

CPP_SOURCES = $(shell find $(SRCDIR) -name '*.cpp')
ASM_SOURCES = $(shell find $(SRCDIR) -name '*.S')
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OUTDIR)/%.o,$(CPP_SOURCES))
OBJECTS += $(patsubst $(SRCDIR)/%.S,$(OUTDIR)/%.S.o,$(ASM_SOURCES))

.PHONY: default help kernel iso qemu gdb clean

default: help
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

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

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
