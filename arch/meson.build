subdir(host_machine.cpu_family())

libarch = static_library(
    'libarch',
    libarch_src,
    include_directories: libarch_include_dirs,
    c_args: ['-std=gnu23', '-Wall', '-Wextra', '-ffreestanding'],
    link_args: libarch_link_args,
    link_depends: libarch_link_depends,
)
