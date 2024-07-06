#pragma once

#include <stddef.h>
#include <stdint.h>

#define GDT_IDX_NULL      0U // Null segment
#define GDT_IDX_KERNEL_CS 1U // Kernel Code segment
#define GDT_IDX_KERNEL_DS 2U // Kernel Data segment
#define GDT_IDX_USER_CS   3U // User Code segment
#define GDT_IDX_USER_DS   4U // User Data segment
#define GDT_NUM_ENTRIES   (GDT_IDX_USER_DS + 1U)

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

/**
 * An entry in the Global Descriptor Table, a.k.a. the Segment Descriptor.
 */
typedef struct [[gnu::packed]] gdt_entry
{
    uint16_t limit_0_15;
    uint16_t base_0_15;
    uint8_t base_16_23;
    uint8_t access;
    uint8_t flags_limit_16_19;
    uint8_t base_24_31;
} gdt_entry_t;

static_assert(sizeof(gdt_entry_t) == 8);

/**
 * Initialize the entries of the Global Descriptor Table.
 */
void gdt_init();

/**
 * Load the Global Descriptor Table with the LGDT instruction.
 */
void gdt_load();
