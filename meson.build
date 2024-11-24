project('t5os', 'c', version: '0.1.0', meson_version: '>= 1.5.0')

# Defines
# - kernel_src
# - kernel_link_depends
# - kernel_include_dirs
# - kernel_link_args
# - kernel_exe
subdir('kernel')

# Run this target with:
#
#   meson compile -C <build-dir> qemu-run-image
#
# Where `<build-dir>` is the directory you configured to cross-compile the
# kernel with the `meson setup` command (such as 'build')
run_target(
    'qemu-run-image',
    command: [
        # This program is defined under the "binaries" section of the
        # selected cross-file config (such as ./meson/cross/i686-elf.ini)
        find_program('qemu'),
        '-kernel', kernel_exe.full_path(),
        '-serial', 'stdio',
        '-m', '64',
    ],
    depends: [kernel_exe],
)
