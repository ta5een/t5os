#pragma once

#include <stddef.h>

typedef size_t (*log_spec_write_t)(size_t len, const char str[len], void *ctx);

struct log_spec
{
    log_spec_write_t write;
    void *ctx;
};

[[gnu::format(printf, 3, 4)]]
void
log_trace(struct log_spec *spec, const char *restrict loc,
          const char *restrict fmt, ...);
