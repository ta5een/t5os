#include <libt5/log.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_LOCATION    ""
#define LOG_BUFFER_SIZE 256

enum
{
    E_OK = 0,
    E_SKIP = 77,
    E_FAIL = 99,
};

char log_buffer[LOG_BUFFER_SIZE] = {0};

static size_t
write(size_t len, const char str[len])
{
    memset(log_buffer, 0, LOG_BUFFER_SIZE);
    strncpy(log_buffer, str, len);
    return len;
}

int
main(void)
{
    struct log_spec spec = {.write = write};

    {
        log_trace(&spec, LOG_LOCATION, "Hello, %s!", "world");
        const char expected_str[] = "Hello, world!";
        if (strncmp(log_buffer, expected_str, strlen(expected_str)) != 0)
        {
            perror("strings do not match");
            exit(E_FAIL);
        }
    }

    {
        log_trace(&spec, LOG_LOCATION, "Hello, %s!", "world");
        const char expected_str[] = "Hello, error!";
        if (strncmp(log_buffer, expected_str, strlen(expected_str)) == 0)
        {
            perror("strings match, when they should not");
            exit(E_FAIL);
        }
    }

    return E_OK;
}
