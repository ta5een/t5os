#include "test_log_common.h"
#include <libt5/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_BUFFER_SIZE 256

static size_t
mock_write(size_t len, const char str[len], struct test_context *ctx)
{
    // strlcpy would have been ideal here, but unfortunately it is not defined
    // in the C standard. The following lines mimic its behaviour, sourced from
    // macOS's strcpy(3) manual.
    strncpy(ctx->log_buffer, str, LOG_BUFFER_SIZE - 1);
    ctx->log_buffer[LOG_BUFFER_SIZE - 1] = '\0';

    // NOTE: Ignoring characters in str beyond LOG_BUFFER_SIZE
    return 0;
}

int
main(void)
{
    char log_buffer[LOG_BUFFER_SIZE] = {'\0'};

    struct test_context context = {
        .log_buffer = log_buffer,
        .log_buffer_size = LOG_BUFFER_SIZE,
    };

    struct log_spec spec = {
        .write = (log_spec_write_t)mock_write,
        .context = &context,
    };

    const char *strings[] = {
        "Hello, world!",
        // "Goodbye, world!",
        // "This is an example string.",
        // "This is another example string.",
        // "This is the final string to test.",
        NULL,
    };

    for (size_t i = 0; strings[i] != NULL; i++)
    {
        clear_log_buffer(&context);

        // Imagine the above strings are passed as literals to log_trace. Since
        // the strings are hard-coded and contain no format specifiers, this is
        // not a security concern.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        log_trace(&spec, LOG_LOCATION_EMPTY, strings[i]);
#pragma GCC diagnostic pop

        if (strcmp(log_buffer, strings[i]) != 0)
        {
            fprintf(stderr, "'%s' SHOULD EQ '%s'\n", log_buffer, strings[i]);
            exit(E_FAIL);
        }
    }

    return E_PASS;
}
