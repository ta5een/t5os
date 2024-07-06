#include "gdt.h"

#define GDT_LIMIT_4KiB 0xfffffU

static gdt_entry_t m_entries[GDT_NUM_ENTRIES];
static descriptor_table_register_t gdtr;

void gdt_write_entry(
    size_t index,
    uint32_t base,
    uint32_t limit,
    uint8_t access,
    uint8_t flags
)
{
    // TODO: Assert index
    gdt_entry_t *entry = &m_entries[index];

    // Encode base
    entry->base_0_15 = base & 0xffffU;
    entry->base_16_23 = (base >> 16U) & 0xffU;
    entry->base_24_31 = (base >> 24U) & 0xffU;

    // Encode limit and flags
    entry->limit_0_15 = limit & 0xffffU;
    entry->flags_limit_16_19 = (flags << 4U) | ((limit >> 16U) & 0xfU);

    // Encode access
    entry->access = access;
}

void gdt_init()
{
    gdt_write_entry(GDT_IDX_NULL, 0U, 0U, 0U, 0U);
    gdt_write_entry(GDT_IDX_KERNEL_CS, 0U, GDT_LIMIT_4KiB, 0x9a, 0xc);
    gdt_write_entry(GDT_IDX_KERNEL_DS, 0U, GDT_LIMIT_4KiB, 0x92, 0xc);
    gdt_write_entry(GDT_IDX_USER_CS, 0U, GDT_LIMIT_4KiB, 0xfa, 0xc);
    gdt_write_entry(GDT_IDX_USER_DS, 0U, GDT_LIMIT_4KiB, 0xf2, 0xc);
    // TODO: Write entry for TSS
}

void gdt_load()
{
    gdtr.limit = (sizeof(gdt_entry_t) * GDT_NUM_ENTRIES) - 1;
    gdtr.base = (void *)m_entries;
    asm volatile("lgdt %0" : : "m"(gdtr) : "memory");
}
