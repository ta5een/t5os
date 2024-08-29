#pragma once

#include <libt5/log.h>
#include <stddef.h>
#include <string.h>

#define LOG_LOCATION_EMPTY ""
#define LOG_LOCATION_FUNC  __FUNCTION__

/**
 * Exit codes for unit tests, following the Meson 'exitcode' testing protocol.
 */
enum
{
    E_PASS = 0,
    E_SKIP = 77,
    E_FAIL = 99,
};

struct test_context
{
    char *log_buffer;
    size_t log_buffer_size;
    struct log_spec spec;
};

void
clear_log_buffer(struct test_context *ctx)
{
    memset(ctx->log_buffer, '\0', ctx->log_buffer_size);
}
