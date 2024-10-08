NAME = t5os
ARCH = i686
BUILD_TYPE ?= DEBUG

OUTDIR = build/kernel
TOOLCHAIN_DIR = toolchain
TOOLCHAIN_BIN_DIR = $(TOOLCHAIN_DIR)/local/$(ARCH)/bin

QEMU ?= qemu-system-i386
QEMUFLAGS := -serial stdio

ifeq ($(BUILD_TYPE),DEBUG)
	QEMUFLAGS += -s -S -d int,cpu_reset -no-shutdown -no-reboot
endif

.PHONY: default help toolchain iso qemu gdb clean

default: help
toolchain: $(TOOLCHAIN_BIN_DIR)/$(ARCH)-elf-*
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
# FIXME: gdb is not available on Apple Silicon systems by default. Consider
# migrating to lldb?
gdb: $(OUTDIR)/$(NAME).bin
	gdb $<

# FIXME: Clean build artifacts with Meson
clean:
	rm -rf $(OUTDIR)/*
