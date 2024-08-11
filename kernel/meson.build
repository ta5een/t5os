kernel_src = []
kernel_link_depends = []
kernel_include_dirs = include_directories('include')

kernel_link_args = [
    '-ffreestanding',
    '-lgcc',
    '-nostdlib',
]

subdir('src')

executable(
    't5os.bin',
    sources: kernel_src,
    include_directories: kernel_include_dirs,
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
    link_args: kernel_link_args,
    link_depends: kernel_link_depends,
)
