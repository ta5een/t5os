#include "libt5/log.h"
#include <stdarg.h>

void
log_trace(struct log_spec *spec, const char *location, const char *fmt, ...)
{
    (void)location;
    (void)fmt;

    va_list list;
    va_start(list, fmt);

    spec->write("Hello, world!", 14);

    va_end(list);
}
