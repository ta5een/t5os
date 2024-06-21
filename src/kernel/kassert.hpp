#pragma once

#include <lib/integers.hpp>

namespace kernel
{

[[noreturn]] void _assert_failure(
    const char *msg,
    const char *file,
    usize line,
    const char *function
);

} // namespace kernel

#define KASSERT(expr)                                                          \
    /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */                     \
    do                                                                         \
    {                                                                          \
        if (!static_cast<bool>(expr)) [[unlikely]]                             \
        {                                                                      \
            kernel::_assert_failure(                                           \
                #expr, __FILE__, __LINE__, __PRETTY_FUNCTION__                 \
            );                                                                 \
        }                                                                      \
    } while (0)
