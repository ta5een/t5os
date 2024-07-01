#pragma once

#include <lib/core.hpp>
#include <lib/integers.hpp>

namespace kernel
{

[[noreturn]]
void _on_assert(const char *msg, const char *location, const char *function);

} // namespace kernel

// TODO: Consider defining a no-op version in case `DEBUG_KERNEL` is false
//
// A constexpr template function is not possible with `__PRETTY_FUNCTION__`.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define KASSERT(expr)                                                          \
    /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */                     \
    do                                                                         \
    {                                                                          \
        if (!static_cast<bool>(expr)) [[unlikely]]                             \
        {                                                                      \
            kernel::_on_assert(                                                \
                #expr,                                                         \
                __FILE__ ":" LIB_STRINGIFY(__LINE__),                          \
                __PRETTY_FUNCTION__                                            \
            );                                                                 \
        }                                                                      \
    } while (0)
