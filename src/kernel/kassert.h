#pragma once

#include <libcore/stringify.h>

[[noreturn]]
void
kernel_on_assert(const char *msg, const char *location, const char *function);

#define KASSERT(expr)                                                          \
    do                                                                         \
    {                                                                          \
        if (!(bool)(expr)) [[unlikely]]                                        \
        {                                                                      \
            kernel_on_assert(                                                  \
                #expr,                                                         \
                __FILE__ ":" LIBCORE_STRINGIFY(__LINE__),                      \
                __PRETTY_FUNCTION__                                            \
            );                                                                 \
        }                                                                      \
    } while (0)
