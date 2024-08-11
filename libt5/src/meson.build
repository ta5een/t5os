libt5_include_dirs = include_directories('../include')

libt5_lib = static_library(
    't5',
    files('log.c'),
    include_directories: libt5_include_dirs,
    c_args: [
        '-std=gnu2x',
        '-Wall',
        '-Wextra',
    ],
)

libt5_dep = declare_dependency(
    link_with: libt5_lib,
    include_directories: libt5_include_dirs,
)
