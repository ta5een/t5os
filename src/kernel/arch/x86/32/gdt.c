#include "gdt.h"

#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/gdt.h>

#define LIMIT_4KiB 0xfffffU

#define ACCESS_CS                                                              \
    (GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE | GDT_ACCESS_PRESENT)
#define ACCESS_DS                                                              \
    (GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE | GDT_ACCESS_PRESENT)
#define ACCESS_KERNEL_CS (GDT_ACCESS_RING0 | ACCESS_CS)
#define ACCESS_KERNEL_DS (GDT_ACCESS_RING0 | ACCESS_DS)
#define ACCESS_USER_CS   (GDT_ACCESS_RING3 | ACCESS_CS)
#define ACCESS_USER_DS   (GDT_ACCESS_RING3 | ACCESS_DS)

#define FLAGS_CS_32 (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)
#define FLAGS_DS_32 (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)

static struct gdt_entry s_gdt[GDT_NUM_ENTRIES];

static const struct descriptor_table_register s_gdtr = {
    .limit = sizeof(s_gdt) - 1,
    .base = (void *)s_gdt
};

/**
 * Loads the Global Descriptor Table with the LGDT instruction.
 *
 * Refer to "gdt.S" for more information.
 */
[[gnu::cdecl]]
extern void
ia32_gdt_load(
    const struct descriptor_table_register *gdtr,
    uint32_t code_segment_selector,
    uint32_t data_segment_selector
);

static inline void
ia32_gdt_set_entry(
    enum gdt_selector selector,
    uint32_t base,
    uint32_t limit,
    enum gdt_access access,
    enum gdt_flag flags
)
{
    s_gdt[GDT_SELECTOR_INDEX(selector)] = (struct gdt_entry){
        .base_0_15 = base & 0xffffU,
        .base_16_23 = (base >> 16U) & 0xffU,
        .base_24_31 = (base >> 24U) & 0xffU,
        .limit_0_15 = limit & 0xffffU,
        .flags_limit_16_19 = (flags << 4U) | ((limit >> 16U) & 0xfU),
        .access = access,
    };
}

void
ia32_gdt_init()
{
    // Set up a minimal GDT using the flat memory model.
    //
    // Since we will primarily use paging for memory protection, this setup
    // defines the essential segment descriptors: a null segment, and separate
    // code and data segments for both kernel and user mode.
    //
    // https://wiki.osdev.org/GDT_Tutorial#Flat_/_Long_Mode_Setup
    ia32_gdt_set_entry(GDT_NULL, 0, 0, 0, 0);
    ia32_gdt_set_entry(GDT_KCODE, 0, LIMIT_4KiB, ACCESS_KERNEL_CS, FLAGS_CS_32);
    ia32_gdt_set_entry(GDT_KDATA, 0, LIMIT_4KiB, ACCESS_KERNEL_DS, FLAGS_DS_32);
    ia32_gdt_set_entry(GDT_UCODE, 0, LIMIT_4KiB, ACCESS_USER_CS, FLAGS_CS_32);
    ia32_gdt_set_entry(GDT_UDATA, 0, LIMIT_4KiB, ACCESS_USER_DS, FLAGS_DS_32);
    // TODO: Write entry for TSS

    ia32_gdt_load(&s_gdtr, GDT_KCODE, GDT_KDATA);
}
