test(
    'test_libt5',
    executable(
        'test_libt5',
        files('log.test.c'),
        native: true,
        include_directories: include_directories('../include'),
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
