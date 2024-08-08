#include "kernel/log.h"
#include <stdarg.h>

void
log_trace(struct log_spec *spec, const char *location, const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);

    while (*fmt != '\0')
    {
        switch (*fmt)
        {
        case '%':
            break;
        default:
            break;
        }

        fmt++;
    }

    va_end(list);
}
