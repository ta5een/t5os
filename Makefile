NAME = wyoos
ARCH = i386
DEBUG ?= 1

SRCDIR = ./src
LIBDIR = ./src
OUTDIR = ./out

CPP_SOURCES = $(shell find $(SRCDIR) -name '*.cpp')
ASM_SOURCES = $(shell find $(SRCDIR) -name '*.S')
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OUTDIR)/%.o,$(CPP_SOURCES))
OBJECTS += $(patsubst $(SRCDIR)/%.S,$(OUTDIR)/%.S.o,$(ASM_SOURCES))

CXXFLAGS := -m32 -std=c++20 -nostdlib -Wall -Wextra -I $(LIBDIR) \
			-fno-builtin -fno-exceptions -fno-rtti -fno-use-cxa-atexit
ASFLAGS =
LDFLAGS =
QEMUFLAGS =

ifeq ($(DEBUG),1)
	# Prepend debug flag to make it more prominent
	CXXFLAGS := -g $(CXXFLAGS)
	QEMUFLAGS += -s -S
endif

# To allow for cross-compilation of ELF binaries, building on Linux will use
# the built-in toolchain with extra flags, whereas building on other OSes (e.g.
# macOS) will default to architecture-specific GNU toolchains.
#
# Inspired by:
#	https://github.com/joexbayer/RetrOS-32/blob/b31c06c39d728d77b2e86496d2bf19d3128dda72/Makefile#L35-L50
UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
	CXX = g++
	AS = as
	LD = ld
	CXXFLAGS += -elf_$(ARCH)
	ASFLAGS += --32
	LDFLAGS += -melf_$(ARCH)
else
	CXX = $(ARCH)-elf-g++
	AS = $(ARCH)-elf-as
	LD = $(ARCH)-elf-ld
endif

.PHONY: kernel iso qemu clean

kernel: $(OUTDIR)/$(NAME).bin
iso: $(OUTDIR)/$(NAME).iso

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OUTDIR)/%.S.o: $(SRCDIR)/%.S
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(OUTDIR)/$(NAME).bin: $(SRCDIR)/kernel/arch/$(ARCH)/linker.ld $(OBJECTS)
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
	qemu-system-$(ARCH) $(QEMUFLAGS) -cdrom $< -m 64

clean:
	rm -rf $(OUTDIR)/*
