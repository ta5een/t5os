project('t5os', 'c', version: '0.1.0', meson_version: '>= 1.5.0')

# libt5 {{{

# Defines:
# - libt5_src
# - libt5_include_dirs
subdir('libt5/src')

libt5_dep = declare_dependency(
    link_with: static_library(
        't5',
        sources: libt5_src,
        include_directories: libt5_include_dirs,
        c_args: [
            '-std=gnu2x',
            '-Wall',
            '-Wextra',
            '-ffreestanding',
        ],
    ),
    include_directories: libt5_include_dirs,
)

# }}}

# kernel {{{

# The kernel will be cross compiled, with libt5 statically linked
if meson.is_cross_build()
    # Defines:
    # - kernel_src
    # - kernel_include_dirs
    # - kernel_link_args
    # - kernel_link_depends
    subdir('kernel/src')

    kernel_exe = executable(
        't5os.elf',
        sources: kernel_src,
        include_directories: [kernel_include_dirs],
        c_args: [
            '-std=gnu2x',
            '-Wall',
            '-Wextra',
            '-ffreestanding',
            # I'm not sure why Meson defines this macro by default, or why it is
            # set to '64', especially since I'm using a 32-bit cross-compiler for
            # now. However, I'm also not sure if there would be any unintended
            # side-effects if this macro is enabled. To be safe, I'll undefine it.
            #
            # PR #3411 suggests adding this flag in the `[built-in options]` or
            # `[properties]` section in the cross file would be sufficient to
            # undefine it. However, this flag doesn't get applied when I do this,
            # and it seems I'm not alone (see: #2303, #3049, #10237). Instead, it
            # appears the most "appropriate" place to undefined it is here.
            #
            # Relevant discussions:
            # - https://github.com/mesonbuild/meson/pull/3411
            # - https://github.com/mesonbuild/meson/issues/2303
            # - https://github.com/mesonbuild/meson/issues/3049
            # - https://github.com/mesonbuild/meson/issues/10237
            '-U_FILE_OFFSET_BITS',
        ],
        dependencies: [libt5_dep],
        link_args: kernel_link_args,
        link_depends: kernel_link_depends,
    )

    # Run this target with:
    #
    #   meson compile -C build qemu-run-image
    #
    # Where `build` is the directory you configured to cross-compile the kernel
    # with the `meson setup` command
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
endif

# }}}

# vim:foldmethod=marker
