#include "kernel/log.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define TEST_EXIT_PASS 0
#define TEST_EXIT_SKIP 77
#define TEST_EXIT_FAIL 99

#define BUFFER_SIZE 256

char buffer[BUFFER_SIZE] = {0};

static void
cleanup()
{
    memset(buffer, 0, BUFFER_SIZE);
}

static size_t
write(const char *str, size_t str_len)
{
    (void)str;
    size_t n_bytes = str_len > BUFFER_SIZE ? BUFFER_SIZE : str_len;
    return n_bytes;
}

int
main(void)
{
    struct log_spec spec = {.write = write};
    (void)spec;

    {
        cleanup();
        const char str[] = "Hello, world!";
        const size_t str_len = 14;
        size_t n_bytes = write(str, str_len);
        if (n_bytes != str_len)
        {
            fprintf(stderr, "nbytes: expected %zu, got %zu", str_len, n_bytes);
            return TEST_EXIT_FAIL;
        }
    }

    /* {
        cleanup();
        const char str[] = "Hello, world!";
        const size_t str_len = 14;
        write(str, str_len);
        if (buffer[0] != str[0])
        {
            fprintf(stderr, "buffer[0]: expected %c, got %c", str[0],
                    buffer[0]);
            return TEST_EXIT_FAIL;
        }
    } */

    return TEST_EXIT_PASS;
}
