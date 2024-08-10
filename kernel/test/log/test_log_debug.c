#include <criterion/criterion.h>
#include <stddef.h>
#include <string.h>

#define BUFFER_SIZE 256

char buffer[BUFFER_SIZE] = {0};

static size_t
write(const char *str, size_t str_len)
{
    (void)str;
    size_t n_bytes = str_len > BUFFER_SIZE ? BUFFER_SIZE : str_len;
    return n_bytes;
}

Test(kernel, log_debug)
{
    const char str[] = "Hello, world!";
    size_t str_len = strlen(str);
    size_t n_bytes = write(str, str_len);
    cr_assert(n_bytes == str_len, "nbytes: expected %zu, got %zu", str_len,
              n_bytes);
}
