#pragma once

#include <stdint.h>

/**
 * A data structure describing a pointer to a particular descriptor table.
 *
 * The structure of this data type is similar in 32-bit and 64-bit mode. The
 * only difference is the size of the base field, where it is 4 bytes long in
 * 32-bit mode and 8 bytes long in 64-bit mode.
 */
struct [[gnu::packed]] x86_descriptor_table_register
{
    /**
     * The size of the descriptor table.
     */
    uint16_t limit;
    /**
     * The linear address of the descriptor table.
     */
    void *base;
};

#if defined(__x86_64__)
// NOLINTNEXTLINE(readability-magic-numbers)
static_assert((bool)(sizeof(struct x86_descriptor_table_register) == 10U));
#elif defined(__i386__)
// NOLINTNEXTLINE(readability-magic-numbers)
static_assert((bool)(sizeof(struct x86_descriptor_table_register) == 6U));
#endif
