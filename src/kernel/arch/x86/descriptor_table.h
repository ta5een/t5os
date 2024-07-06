#pragma once

#include <stdint.h>

/**
 * A data structure describing a pointer to a particular descriptor table.
 */
typedef struct [[gnu::packed]] descriptor_table_register
{
    /**
     * The size of the descriptor table.
     */
    uint16_t limit;
    /**
     * The linear address of the descriptor table.
     */
    void *base;
} descriptor_table_register_t;

#if defined(__i386__) && !defined(__x86_64__)
static_assert(sizeof(descriptor_table_register_t) == 6);
#elif defined(__x86_64__)
static_assert(sizeof(descriptor_table_register_t) == 10);
#endif
