libarch_src = []
libarch_link_args = []
libarch_link_depends = []
libarch_include_dirs = include_directories('include')  # Parent directory

libarch_src += files(
    'src/cpu.c',
    'src/devices/serial.c',
    'src/devices/vga.c',
    'src/port.c',
)

if host_machine.cpu() == 'x86'
    libarch_src += files(
        'src/32/boot.S',
        'src/32/gdt.S',
        'src/32/gdt.c',
        'src/32/idt.c',
        'src/32/isr.S',
        'src/32/isr.c',
    )

    libarch_link_depends += files('src/32/linker.ld')
    libarch_link_args += ['-T', libarch_link_depends.get(0).full_path()]
endif
