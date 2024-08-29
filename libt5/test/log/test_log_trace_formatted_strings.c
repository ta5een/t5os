#include "test_log_common.h"
#include <libt5/log.h>
#include <string.h>

#define LOG_BUFFER_SIZE 256

static size_t
mock_write(size_t len, const char str[len], struct test_context *ctx)
{
    strncpy(ctx->log_buffer, str, len);
    return len;
}

int
main(void)
{
    char log_buffer[LOG_BUFFER_SIZE] = {'\0'};

    struct test_context ctx = {
        .log_buffer = log_buffer,
        .log_buffer_size = LOG_BUFFER_SIZE,
        .spec = {NULL, NULL},
    };

    ctx.spec.write = (log_spec_write_t)mock_write;
    ctx.spec.ctx = &ctx;

    return E_SKIP;
}
