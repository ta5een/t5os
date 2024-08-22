#pragma once

#include <stddef.h>

struct log_spec
{
    size_t (*write)(size_t len, const char str[len]);
};

[[gnu::format(printf, 3, 4)]]
void
log_trace(struct log_spec *spec, const char *restrict loc,
          const char *restrict fmt, ...);
