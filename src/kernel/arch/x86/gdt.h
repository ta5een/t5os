#pragma once

#include <stddef.h>
#include <stdint.h>

#define GDT_NUM_ENTRIES (GDT_IDX_UDATA + 1U)

enum gdt_entry_index : uint8_t
{
    /**
     * Null segment
     */
    GDT_IDX_NULL = 0U,
    /**
     * Kernel Mode code segment
     */
    GDT_IDX_KCODE = 1U,
    /**
     * Kernel Mode data segment
     */
    GDT_IDX_KDATA = 2U,
    /**
     * User Mode code segment
     */
    GDT_IDX_UCODE = 3U,
    /**
     * User Mode data segment
     */
    GDT_IDX_UDATA = 4U,
};

enum gdt_access : uint8_t
{
    GDT_ACCESS_ACCESSED = 1U,
    GDT_ACCESS_WRITEABLE = 1U << 1U,
    GDT_ACCESS_CONFORMING = 1U << 2U,
    GDT_ACCESS_EXECUTABLE = 1U << 3U,
    GDT_ACCESS_NOT_SYSTEM = 1U << 4U,
    GDT_ACCESS_RING0 = 0U << 5U,
    GDT_ACCESS_RING1 = 1U << 5U,
    GDT_ACCESS_RING2 = 2U << 5U,
    GDT_ACCESS_RING3 = 3U << 5U,
    GDT_ACCESS_PRESENT = 1U << 7U,
};

enum gdt_flag : uint8_t
{
    GDT_FLAG_LONG_MODE = 1U << 1U,
    GDT_FLAG_DEFAULT_SIZE = 1U << 2U,
    GDT_FLAG_GRANULARITY = 1U << 3U,
};

/**
 * An entry in the Global Descriptor Table, representing a Segment Descriptor.
 */
struct [[gnu::packed]] gdt_entry
{
    uint16_t limit_0_15;
    uint16_t base_0_15;
    uint8_t base_16_23;
    uint8_t access;
    uint8_t flags_limit_16_19;
    uint8_t base_24_31;
};

// NOLINTNEXTLINE(readability-*)
static_assert(sizeof(struct gdt_entry) == 8U);

struct gdt
{
    struct gdt_entry entries[GDT_NUM_ENTRIES];
};

/**
 * Initializes the entries of the Global Descriptor Table.
 */
void
gdt_init(struct gdt *gdt);

/**
 * Loads the Global Descriptor Table with the LGDT instruction.
 */
void
gdt_load(const struct gdt gdt[static 1]);
