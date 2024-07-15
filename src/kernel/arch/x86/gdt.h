#pragma once

#include <stddef.h>
#include <stdint.h>

// #define GDT_NUM_ENTRIES     (GDT_IDX_UDATA + 1U)
// #define GDT_SELECTOR(index) (index * sizeof(struct gdt_entry))

// TODO: Hard-coded for now
#define GDT_NUM_ENTRIES (6)

// enum gdt_entry_index : uint8_t
// {
//     /**
//      * Null segment
//      */
//     GDT_IDX_NULL = 0U,
//     /**
//      * Kernel Mode code segment
//      */
//     GDT_IDX_KCODE = 1U,
//     /**
//      * Kernel Mode data segment
//      */
//     GDT_IDX_KDATA = 2U,
//     /**
//      * User Mode code segment
//      */
//     GDT_IDX_UCODE = 3U,
//     /**
//      * User Mode data segment
//      */
//     GDT_IDX_UDATA = 4U,
// };

enum gdt_access : uint8_t
{
    GDT_ACCESS_DATA_WRITEABLE = 1U << 1U,
    GDT_ACCESS_CODE_READABLE = 1U << 1U,

    GDT_ACCESS_DATA_DIRECTION_UP = 0U << 2U,
    GDT_ACCESS_DATA_DIRECTION_DOWN = 1U << 2U,
    GDT_ACCESS_CODE_CONFORMING = 1U << 2U,

    GDT_ACCESS_DATA_SEGMENT = 0b10U << 3U,
    GDT_ACCESS_CODE_SEGMENT = 0b11U << 3U,

    GDT_ACCESS_RING0 = 0U << 5U,
    GDT_ACCESS_RING1 = 1U << 5U,
    GDT_ACCESS_RING2 = 2U << 5U,
    GDT_ACCESS_RING3 = 3U << 5U,

    GDT_ACCESS_PRESENT = 1U << 7U,
};

enum gdt_flag : uint8_t
{
    GDT_FLAG_16BIT = 0b00U << 1U,
    GDT_FLAG_32BIT = 0b10U << 1U,
    GDT_FLAG_64BIT = 0b01U << 1U,

    GDT_FLAG_GRANULARITY_1B = 0U << 3U,
    GDT_FLAG_GRANULARITY_4K = 1U << 3U,
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

/**
 * Initializes the entries of the Global Descriptor Table.
 */
void
gdt_init();

/**
 * Loads the Global Descriptor Table with the LGDT instruction.
 */
void
gdt_load();
