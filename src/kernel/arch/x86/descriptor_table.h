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

#if defined(__i386__) && !defined(__x86_64__)
static_assert(sizeof(struct descriptor_table_register) == 6);
#elif defined(__x86_64__)
static_assert(sizeof(struct descrdescriptor_table_register) == 10);
#endif
