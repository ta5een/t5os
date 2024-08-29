#pragma once

#include <stddef.h>

typedef size_t (*log_spec_write_t)(size_t len, const char str[len], void *ctx);

struct log_spec
{
    /**
     * Function to write a UTF-8 encoded `str` to some implementation-defined
     * log buffer.
     *
     * Returns the remaining number of bytes not written to the log buffer. This
     * may be the case when the buffer did not have enough capacity to write the
     * whole string.
     */
    [[nodiscard("buffer is not guaranteed to write the whole string")]]
    log_spec_write_t write;
    /**
     * Payload passed as the final argument to `log_spec.write`.
     */
    void *context;
};

[[gnu::format(printf, 3, 4)]]
void
log_trace(struct log_spec *spec, const char *restrict loc,
          const char *restrict fmt, ...);
