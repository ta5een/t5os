arch_src += files(
    'src/cpu.c',
    'src/devices/serial.c',
    'src/devices/vga.c',
    'src/port.c',
)

# "x86" for 32-bit, "x86_64" for 64-bit
if host_machine.cpu() == 'x86'
    arch_src += files(
        'src/32/boot.S',
        'src/32/gdt.S',
        'src/32/gdt.c',
        'src/32/idt.c',
        'src/32/isr.S',
        'src/32/isr.c',
    )

    arch_link_depends += files('src/32/linker.ld')
    arch_link_args += ['-T', arch_link_depends.get(0).full_path()]
endif
