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
 * An entry in the Global Descriptor Table, representing a Segment Descriptor.
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

static_assert(sizeof(gdt_entry_t) == 8U);

typedef struct gdt
{
    gdt_entry_t entries[GDT_NUM_ENTRIES];
} gdt_t;

/**
 * Initialize the entries of the Global Descriptor Table.
 */
void gdt_init(gdt_t *gdt);

/**
 * Load the Global Descriptor Table with the LGDT instruction.
 */
void gdt_load(const gdt_t gdt[static 1]);
