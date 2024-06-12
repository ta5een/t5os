# NOTE: To allow for cross-compilation, please override the `CXX`, `AS` and `LD`
# environment variables. For example, you may use `CXX=i386-elf-g++` on macOS.

NAME = wyoos
ARCH = i386

SRCDIR = ./src
LIBDIR = ./lib
OUTDIR = ./out

CPP_SOURCES = $(shell find $(SRCDIR) -name '*.cpp')
ASM_SOURCES = $(shell find $(SRCDIR) -name '*.asm')
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OUTDIR)/%.o,$(CPP_SOURCES))
OBJECTS += $(patsubst $(SRCDIR)/%.asm,$(OUTDIR)/%.asm.o,$(ASM_SOURCES))

CXXFLAGS = -m32 -Wall -Wextra -nostdlib -I $(LIBDIR) -fno-builtin \
		   -fno-exceptions -fno-rtti -fno-use-cxa-atexit
ASFLAGS = --32
LDFLAGS = -melf_$(ARCH)

.PHONY: all binary iso clean

all: iso
binary: $(OUTDIR)/$(NAME).bin
iso: $(OUTDIR)/$(NAME).iso

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OUTDIR)/%.asm.o: $(SRCDIR)/%.asm
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(OUTDIR)/$(NAME).bin: $(SRCDIR)/arch/$(ARCH)/linker.ld $(OBJECTS)
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
	qemu-system-$(ARCH) -cdrom $< -m 64

clean:
	rm -rf $(OUTDIR)/*
