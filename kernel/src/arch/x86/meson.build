kernel_src += files(
    'devices/serial.c',
    'devices/vga.c',
    'cpu.c',
    'port.c',
)

if host_machine.cpu() == 'x86'
    kernel_src += files(
        '32/boot.S',
        '32/gdt.S',
        '32/gdt.c',
        '32/idt.c',
        '32/isr.S',
        '32/isr.c',
    )

    kernel_link_depends += files('32/linker.ld')
    kernel_link_args += ['-T', kernel_link_depends.get(0).full_path()]
endif
