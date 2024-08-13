kernel_src = []
kernel_link_depends = []
kernel_include_dirs = include_directories('../include')

kernel_link_args = [
    '-ffreestanding',
    '-lgcc',
    '-nostdlib',
]

kernel_src += files(
    'kassert.c',
    'kernel.c',
    'kpanic.c',
)

if host_machine.cpu_family() == 'x86'
    kernel_src += files(
        'arch/x86/cpu.c',
        'arch/x86/devices/serial.c',
        'arch/x86/devices/vga.c',
        'arch/x86/port.c',
    )

    if host_machine.cpu() == 'x86'
        kernel_src += files(
            'arch/x86/32/boot.S',
            'arch/x86/32/gdt.S',
            'arch/x86/32/gdt.c',
            'arch/x86/32/idt.c',
            'arch/x86/32/isr.S',
            'arch/x86/32/isr.c',
        )

        kernel_link_depends += files('arch/x86/32/linker.ld')
        kernel_link_args += ['-T', kernel_link_depends.get(0).full_path()]
    endif
endif
