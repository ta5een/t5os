#include "kernel/arch/x86/gdt.h"
#include "kernel/arch/x86/descriptor_table.h"
#include <stdint.h>

#define LIMIT_4KiB 0xfffffU

#define ACCESS_CS                                                              \
    (GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE | GDT_ACCESS_PRESENT)
#define ACCESS_DS                                                              \
    (GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE | GDT_ACCESS_PRESENT)
#define ACCESS_KCODE (GDT_ACCESS_RING0 | ACCESS_CS)
#define ACCESS_KDATA (GDT_ACCESS_RING0 | ACCESS_DS)
#define ACCESS_UCODE (GDT_ACCESS_RING3 | ACCESS_CS)
#define ACCESS_UDATA (GDT_ACCESS_RING3 | ACCESS_DS)

#define FLAGS_CS_32 (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)
#define FLAGS_DS_32 (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)

static struct x86_gdt_entry s_gdt[GDT_NUM_ENTRIES];

static const struct x86_descriptor_table_register s_gdtr = {
    .limit = sizeof(s_gdt) - 1,
    .base = (void *)s_gdt,
};

/**
 * Loads the Global Descriptor Table with the LGDT instruction.
 *
 * Refer to "gdt.S" for more information.
 */
[[gnu::cdecl]]
extern void
x86_32_gdt_load(const struct x86_descriptor_table_register *gdtr,
                uint32_t code_segment_selector, uint32_t data_segment_selector);

static inline void
x86_32_gdt_set_entry(enum x86_gdt_selector selector, uint32_t base,
                     uint32_t limit, enum x86_gdt_access access, uint8_t flags)
{
    s_gdt[GDT_SELECTOR_INDEX(selector)] = (struct x86_gdt_entry){
        .base_0_15 = base & 0xffffU,
        .base_16_23 = (base >> 16U) & 0xffU,
        .base_24_31 = (base >> 24U) & 0xffU,
        .limit_0_15 = limit & 0xffffU,
        .flags_limit_16_19 = (flags << 4U) | ((limit >> 16U) & 0xfU),
        .access = access,
    };
}

static inline void
x86_32_gdt_init()
{
    // Set up a minimal GDT using the flat memory model.
    //
    // Since we will primarily use paging for memory protection, this setup
    // defines the essential segment descriptors: a null segment, and separate
    // code and data segments for both kernel and user mode.
    //
    // https://wiki.osdev.org/GDT_Tutorial#Flat_/_Long_Mode_Setup
    x86_32_gdt_set_entry(GDT_NULL, 0U, 0U, 0U, 0U);
    x86_32_gdt_set_entry(GDT_KCODE, 0U, LIMIT_4KiB, ACCESS_KCODE, FLAGS_CS_32);
    x86_32_gdt_set_entry(GDT_KDATA, 0U, LIMIT_4KiB, ACCESS_KDATA, FLAGS_DS_32);
    x86_32_gdt_set_entry(GDT_UCODE, 0U, LIMIT_4KiB, ACCESS_UCODE, FLAGS_CS_32);
    x86_32_gdt_set_entry(GDT_UDATA, 0U, LIMIT_4KiB, ACCESS_UDATA, FLAGS_DS_32);
    // TODO: Write entry for TSS

    x86_32_gdt_load(&s_gdtr, GDT_KCODE, GDT_KDATA);
}

void
x86_gdt_init()
{
    x86_32_gdt_init();
}
