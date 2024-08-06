arch_include_dirs += include_directories('include')

arch_src += files(
    'src/boot.S',
    'src/gdt.S',
    'src/gdt.c',
    'src/idt.c',
    'src/isr.S',
    'src/isr.c',
)

arch_link_depends += files('src/linker.ld')
arch_link_args += ['-T', arch_link_depends.get(0).full_path()]
