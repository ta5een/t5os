project('t5os', 'c', version: '0.1.0', meson_version: '>= 1.5.0')

subdir('libt5')

libt5_dep = declare_dependency(
    link_with: static_library(
        't5',
        sources: libt5_src,
        include_directories: libt5_include_dirs,
        c_args: [
            '-std=gnu2x',
            '-Wall',
            '-Wextra',
        ],
    ),
    include_directories: libt5_include_dirs,
)

# Kernel
# The kernel will be cross compiled, with libt5 statically linked
if meson.is_cross_build()
    subdir('kernel')

    executable(
        't5os.bin',
        sources: kernel_src,
        include_directories: [kernel_include_dirs],
        c_args: [
            '-std=gnu23',
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
endif

# Testing
# The C standard library is currently not available in the target environment,
# so the test executables will only be built for the build machine.
if not meson.is_cross_build()
    test_libt5_src = []
    subdir('libt5/test')

    test(
        'test_libt5',
        executable(
            'test_libt5',
            test_libt5_src,
            native: true,
            include_directories: libt5_include_dirs,
            c_args: [
                '-std=gnu2x',
                '-Wall',
                '-Wextra',
            ],
            dependencies: [
                libt5_dep,
                dependency(
                    'criterion',
                    native: true,
                    fallback: ['criterion', 'criterion'],
                    version: '>=2.4.2',
                ),
            ],
        ),
        suite: 'libt5',
    )
endif
