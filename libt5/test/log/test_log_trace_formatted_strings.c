#include "test_log_common.h"
#include <libt5/log.h>
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

    (void)context;

    return E_SKIP;
}
