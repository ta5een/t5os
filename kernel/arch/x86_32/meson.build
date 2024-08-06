arch_src += files(
    'boot.S',
    'gdt.S',
    'gdt.c',
    'idt.c',
    'isr.S',
    'isr.c',
)

arch_link_depends += files('linker.ld')
arch_link_args += ['-T', arch_link_depends.get(0).full_path()]
