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

    // TODO: Use of hard-coded string
    const char str_hello[] = "Hello, world!";
    // TODO: sizeof is used here since strlen has not been implemented yet
    spec->write(sizeof(str_hello), str_hello, spec->ctx);

    va_end(list);
}
