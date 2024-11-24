#include "libt5/log.h"
#include <stdarg.h>
#include <stdatomic.h>

static size_t
log_spec_writer_noop(size_t /*len*/, const char * /*str[len]*/, void * /*ctx*/);

static _Atomic log_spec_write_t writer = log_spec_writer_noop;

log_spec_write_t
log_spec_get_writer()
{
    return atomic_load(&writer);
}

void
log_spec_set_writer(log_spec_write_t new_writer)
{
    if (new_writer == NULL)
    {
        new_writer = log_spec_writer_noop;
    }

    atomic_store(&writer, new_writer);
}

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
    size_t bytes = spec->write(sizeof(str_hello), str_hello, spec->context);
    // TODO: What to do with the remaining bytes?
    (void)bytes;

    va_end(list);
}

static size_t
log_spec_writer_noop(size_t /*len*/, const char * /*str[len]*/, void * /*ctx*/)
{
    return 0;
}
