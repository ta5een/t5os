#pragma once

#include <stddef.h>
#include <stdint.h>

#define GDT_IDX_NULL      0U // Null segment
#define GDT_IDX_KERNEL_CS 1U // Kernel Code segment
#define GDT_IDX_KERNEL_DS 2U // Kernel Data segment
#define GDT_IDX_USER_CS   3U // User Code segment
#define GDT_IDX_USER_DS   4U // User Data segment
#define GDT_NUM_ENTRIES   (GDT_IDX_USER_DS + 1U)

#define GDT_ACCESS_ACCESS     1U
#define GDT_ACCESS_WRITEABLE  1U << 1U
#define GDT_ACCESS_CONFORMING 1U << 2U
#define GDT_ACCESS_EXECUTABLE 1U << 3U
#define GDT_ACCESS_NOT_SYSTEM 1U << 4U
#define GDT_ACCESS_RING0      0U << 5U
#define GDT_ACCESS_RING1      1U << 5U
#define GDT_ACCESS_RING2      2U << 5U
#define GDT_ACCESS_RING3      3U << 5U
#define GDT_ACCESS_PRESENT    1U << 7U

#define GDT_FLAG_LONG_MODE    1U << 1U
#define GDT_FLAG_DEFAULT_SIZE 1U << 2U
#define GDT_FLAG_GRANULARITY  1U << 3U

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
