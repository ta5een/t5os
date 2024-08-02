#pragma once

#include "kernel/stringify.h"

[[noreturn]]
void
kernel_on_panic(const char *msg, const char *location, const char *function);

#define KPANIC(msg)                                                            \
    do                                                                         \
    {                                                                          \
        kernel_on_panic(#msg, __FILE__ ":" STRINGIFY(__LINE__),                \
                        __PRETTY_FUNCTION__);                                  \
    } while (0)
