#include <criterion/criterion.h>
#include <libt5/log.h>
#include <stddef.h>
#include <string.h>

#define LOG_LOCATION    ""
#define LOG_BUFFER_SIZE (1 << 8)

char log_buffer[LOG_BUFFER_SIZE] = {0};

static size_t
write(size_t len, const char str[len])
{
    memset(log_buffer, 0, LOG_BUFFER_SIZE);
    strncpy(log_buffer, str, len);
    return len;
}

Test(libt5, log_trace)
{
    struct log_spec spec = {.write = write};
    log_trace(&spec, LOG_LOCATION, "");
    cr_assert_str_eq(log_buffer, "Hello, world!");
}

Test(libt5, log_debug)
{
    cr_assert_eq(1, 1);
}
