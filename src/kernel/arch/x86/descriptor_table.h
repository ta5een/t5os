#pragma once

#include <stdint.h>

/**
 * A data structure describing a pointer to a particular descriptor table.
 */
struct [[gnu::packed]] descriptor_table_register
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
static_assert((bool)(sizeof(struct descriptor_table_register) == 10U));
#elif defined(__i386__)
// NOLINTNEXTLINE(readability-magic-numbers)
static_assert((bool)(sizeof(struct descriptor_table_register) == 6U));
#endif
