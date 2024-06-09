OUTDIR = ./bin
OBJECTS = kernel.o loader.o

CC = i386-elf-g++
AS = i386-elf-as
LD = i386-elf-ld
CCFLAGS = -m32 -Wall
AS_FLAGS = --32
LD_FLAGS = -melf_i386

$(OUTDIR)/%.o: %.cpp
	$(CC) $(CC_FLAGS) -o $@ -c $<

$(OUTDIR)/%.o: %.s
	$(AS) $(AS_FLAGS) -o $@ $<

$(OUTDIR)/wyoos.bin: linker.ld $(OBJECTS)
	$(LD) $(LD_FLAGS) -T $< -o $@ $(OBJECTS)
