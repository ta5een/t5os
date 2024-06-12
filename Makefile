NAME = wyoos
OUTDIR = ./out
OBJECTS = $(OUTDIR)/kernel.o $(OUTDIR)/loader.o

UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
	CC = g++
	AS = as
	LD = ld
else
	CC = i386-elf-g++
	AS = i386-elf-as
	LD = i386-elf-ld
endif

CC_FLAGS = -m32 -Wall -nostdlib -fno-builtin -fno-exceptions \
		   -fno-leading-underscore -fno-rtti -fno-use-cxa-atexit
AS_FLAGS = --32
LD_FLAGS = -melf_i386

.PHONY: all clean

all: $(OUTDIR)/$(NAME).iso

$(OUTDIR)/%.o: %.cpp
	$(CC) $(CC_FLAGS) -o $@ -c $<

$(OUTDIR)/%.o: %.s
	$(AS) $(AS_FLAGS) -o $@ $<

$(OUTDIR)/$(NAME).bin: linker.ld $(OBJECTS)
	$(LD) $(LD_FLAGS) -T $< -o $@ $(OBJECTS)

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
	qemu-system-i386 -cdrom $< -m 64

clean:
	rm -f $(OUTDIR)/*
