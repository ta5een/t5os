#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/gdt.h>

#define LIMIT_4KiB 0xfffffU

#define ACCESS_DS                                                              \
    (GDT_ACCESS_WRITEABLE | GDT_ACCESS_NOT_SYSTEM | GDT_ACCESS_PRESENT)
#define ACCESS_CS        (ACCESS_DS | GDT_ACCESS_EXECUTABLE)
#define ACCESS_KERNEL_CS (GDT_ACCESS_RING0 | ACCESS_CS)
#define ACCESS_KERNEL_DS (GDT_ACCESS_RING0 | ACCESS_DS)
#define ACCESS_USER_CS   (GDT_ACCESS_RING3 | ACCESS_CS)
#define ACCESS_USER_DS   (GDT_ACCESS_RING3 | ACCESS_DS)

#define FLAGS_DS_32 (GDT_FLAG_GRANULARITY | GDT_FLAG_DEFAULT_SIZE)
#define FLAGS_CS_32 (FLAGS_DS_32)
#define FLAGS_DS_64 (GDT_FLAG_GRANULARITY | GDT_FLAG_DEFAULT_SIZE)
#define FLAGS_CS_64 (GDT_FLAG_GRANULARITY | GDT_FLAG_LONG_MODE)

void gdt_set_entry(
    struct gdt_entry target[static 1],
    uint32_t base,
    uint32_t limit,
    enum gdt_access access,
    enum gdt_flag flags
)
{
    // Encode base
    target->base_0_15 = base & 0xffffU;
    target->base_16_23 = (base >> 16U) & 0xffU;
    target->base_24_31 = (base >> 24U) & 0xffU;

    // Encode limit and flags
    target->limit_0_15 = limit & 0xffffU;
    target->flags_limit_16_19 = (flags << 4U) | ((limit >> 16U) & 0xfU);

    // Encode access
    target->access = access;
}

void gdt_init(struct gdt *gdt)
{
    // Null segment
    gdt_set_entry(&gdt->entries[GDT_IDX_NULL], 0U, 0U, 0U, 0U);
    // Kernel Mode Code segment
    gdt_set_entry(
        &gdt->entries[GDT_IDX_KCODE],
        0U,
        LIMIT_4KiB,
        ACCESS_KERNEL_CS,
        FLAGS_CS_32
    );
    // Kernel Mode Data segment
    gdt_set_entry(
        &gdt->entries[GDT_IDX_KDATA],
        0U,
        LIMIT_4KiB,
        ACCESS_KERNEL_DS,
        FLAGS_DS_32
    );
    // User Mode Code segment
    gdt_set_entry(
        &gdt->entries[GDT_IDX_UCODE],
        0U,
        LIMIT_4KiB,
        ACCESS_USER_CS,
        FLAGS_CS_32
    );
    // User Mode Data segment
    gdt_set_entry(
        &gdt->entries[GDT_IDX_UDATA],
        0U,
        LIMIT_4KiB,
        ACCESS_USER_DS,
        FLAGS_DS_32
    );
    // TODO: Write entry for TSS
}

void gdt_load(const struct gdt gdt[static 1])
{
    struct descriptor_table_register gdtr;
    gdtr.limit = (sizeof(struct gdt) * GDT_NUM_ENTRIES) - 1;
    gdtr.base = (void *)gdt->entries;
    asm volatile("lgdt %0" : : "m"(gdtr) : "memory");
}
