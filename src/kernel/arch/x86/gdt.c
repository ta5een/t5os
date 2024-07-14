#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/gdt.h>
#include <stddef.h>

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
#define FLAGS_CS_64 (GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K)
// NOTE: Not sure why OSDev Wiki instructs setting the data segment flag to 0xc
// for 64-bit data segments:
// https://wiki.osdev.org/GDT_Tutorial#Flat_/_Long_Mode_Setup
#define FLAGS_DS_64 (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)

static struct gdt_entry s_gdt[GDT_NUM_ENTRIES];

static struct descriptor_table_register s_gdtr = {
    .limit = sizeof(s_gdt) - 1,
    .base = (void *)s_gdt
};

void
gdt_set_entry(
    size_t index,
    uint32_t base,
    uint32_t limit,
    enum gdt_access access,
    enum gdt_flag flags
)
{
    s_gdt[index] = (struct gdt_entry){
        .base_0_15 = base & 0xffffU,
        .base_16_23 = (base >> 16U) & 0xffU,
        .base_24_31 = (base >> 24U) & 0xffU,
        .limit_0_15 = limit & 0xffffU,
        .flags_limit_16_19 = (flags << 4U) | ((limit >> 16U) & 0xfU),
        .access = access,
    };
}

void
gdt_init()
{
    gdt_set_entry(GDT_IDX_NULL, 0U, 0U, 0U, 0U);
    gdt_set_entry(GDT_IDX_KCODE, 0U, LIMIT_4KiB, ACCESS_KERNEL_CS, FLAGS_CS_32);
    gdt_set_entry(GDT_IDX_KDATA, 0U, LIMIT_4KiB, ACCESS_KERNEL_DS, FLAGS_DS_32);
    gdt_set_entry(GDT_IDX_UCODE, 0U, LIMIT_4KiB, ACCESS_USER_CS, FLAGS_CS_32);
    gdt_set_entry(GDT_IDX_UDATA, 0U, LIMIT_4KiB, ACCESS_USER_DS, FLAGS_DS_32);
    // TODO: Write entry for TSS
}

void
gdt_load()
{
    asm volatile("lgdt %0" : : "m"(s_gdtr) : "memory");
}
