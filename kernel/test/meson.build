test(
    'test log debug',
    executable(
        'test_log_debug',
        native: true,
        sources: files(
            'log/test_log_debug.c',
        ),
        include_directories: include_directories('../include'),
        c_args: [
            '-std=gnu2x',
            '-Wall',
            '-Wextra',
        ],
        dependencies: [
            dependency(
                'criterion',
                native: true,
                fallback: ['criterion', 'criterion'],
                version: '>=2.4.2',
            ),
        ],
    ),
    suite: 'kernel',
)
