#include "test_log_common.h"
#include <libt5/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_BUFFER_SIZE 256

char log_buffer[LOG_BUFFER_SIZE] = {'\0'};

static size_t
write(size_t len, const char str[len])
{
    strncpy(log_buffer, str, len);
    return len;
}

int
main(void)
{
    struct test_context context = {
        .log_buffer = log_buffer,
        .log_buffer_size = LOG_BUFFER_SIZE,
        .spec = {.write = write},
    };

    const char *strings[] = {"Hello, world!",
                             // "Goodbye, world!",
                             // "This is an example string.",
                             // "This is another example string.",
                             // "This is the final string to test.",
                             NULL};

    for (size_t i = 0; strings[i] != NULL; i++)
    {
        clear_log_buffer(&context);
        log_trace(&context.spec, LOG_LOCATION_EMPTY, "%s", strings[i]);
        if (strcmp(log_buffer, strings[i]) != 0)
        {
            fprintf(stderr, "'%s' SHOULD EQ '%s'\n", log_buffer, strings[i]);
            exit(E_FAIL);
        }
    }

    return E_PASS;
}
