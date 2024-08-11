#pragma once

#include <stddef.h>

struct log_spec
{
    size_t (*write)(const char *str, size_t n_chars);
};

[[gnu::format(printf, 3, 4)]]
void
log_trace(struct log_spec *spec, const char *location, const char *fmt, ...);
