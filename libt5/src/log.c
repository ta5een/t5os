#include "libt5/log.h"
#include <stdarg.h>

enum printf_specifier
{
    SPECIFIER_NONE,
    SPECIFIER_INT_DECIMAL = 'd',
    SPECIFIER_INT_DECIMAL_ALT = 'i',
    SPECIFIER_UINT_DECIMAL = 'u',
    SPECIFIER_UINT_OCTAL = 'o',
    SPECIFIER_UINT_HEXADECIMAL = 'x',
    SPECIFIER_UINT_HEXADECIMAL_UPPER = 'X',
    SPECIFIER_CHARACTER = 'c',
    SPECIFIER_STRING = 's',
    SPECIFIER_POINTER = 'p',
};

enum printf_flag
{
    FLAG_JUSTIFY_LEFT,
    FLAG_WRITE_SIGN,
    FLAG_LEFT_PAD_WITH_ZERO,
    FLAG_BLANK,
    FLAG_OCTOTHORP,
};

struct printf_args
{
    char flags;
    size_t width;
    size_t precision;
};

void
vlog_trace(struct log_spec *spec, const char *restrict loc,
           const char *restrict fmt, va_list args)
{
    size_t idx = 0;
    char byte = '\0';
    enum printf_specifier state = SPECIFIER_NONE;

    while ((byte = fmt[idx]) != '\0')
    {
        if (byte == '%')
        {
            // TODO: Consume format specifier and set appropriate state
            // continue;
        }
        else if (state == SPECIFIER_CHARACTER)
        {
            char value = va_arg(args, unsigned int);
            (void)spec->write(1, &value, spec->context);
        }
        else
        {
            // TODO: Print out current byte
            (void)spec->write(1, &byte, spec->context);
        }
    }
}

void
log_trace(struct log_spec *spec, const char *restrict loc,
          const char *restrict fmt, ...)
{
    (void)loc;
    (void)fmt;

    va_list list;
    va_start(list, fmt);

    // // TODO: Use of hard-coded string
    // const char str_hello[] = "Hello, world!";
    // // TODO: sizeof is used here since strlen has not been implemented yet
    // size_t bytes = spec->write(sizeof(str_hello), str_hello, spec->context);
    // // TODO: What to do with the remaining bytes?
    // (void)bytes;

    vlog_trace(spec, loc, fmt, list);

    va_end(list);
}
