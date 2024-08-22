#include "libt5/log.h"
#include <stdarg.h>

void
log_trace(struct log_spec *spec, const char *restrict loc,
          const char *restrict fmt, ...)
{
    (void)loc;
    (void)fmt;

    va_list list;
    va_start(list, fmt);

    const char str_hello[] = "Hello, world!";
    spec->write(sizeof(str_hello), str_hello);

    va_end(list);
}
