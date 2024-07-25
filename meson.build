project('t5os', 'c', version: '0.1.0', meson_version: '>= 1.5.0')

kernel_src = []
subdir('kernel')

kernel_exe = executable(
    't5os.bin',
    sources: kernel_src,
    include_directories: include_directories('kernel'),
    c_args: ['-std=gnu23', '-Wall', '-Wextra', '-ffreestanding'],
    link_args: [
        '-Wl,-T' + meson.current_source_dir() + '/kernel/arch/x86/32/linker.ld',
        '-ffreestanding',
        '-lgcc',
        '-nostdlib',
    ],
    link_depends: files('kernel/arch/x86/32/linker.ld'),
)
