#include <kernel/arch/x86/descriptor_table.h>
#include <kernel/arch/x86/devices/vga.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>
#include <kernel/arch/x86/ports.h>
#include <stddef.h>
#include <stdint.h>

#define PORT_PIC_MASTER_COMMAND (0x20U)
#define PORT_PIC_MASTER_DATA    (0x21U)
#define PORT_PIC_SLAVE_COMMAND  (0xa0U)
#define PORT_PIC_SLAVE_DATA     (0xa1U)

#define ISR_NUM_DEFINED_HANDLERS (0x1fU + 1U)
#define ISR_HANDLER_ALIGNMENT    (16U)

extern uint8_t isr_0x00[];

typedef void (*isr_handler_t)(void);
extern isr_handler_t isr_unhandled_stub;

static struct idt_entry s_idt[IDT_NUM_ENTRIES];

static const struct descriptor_table_register s_idtr = {
    .limit = sizeof(s_idt) - 1,
    .base = (void *)s_idt,
};

void
idt_set_entry(
    size_t index,
    size_t segment_index,
    uint32_t handler,
    enum idt_flag flags
)
{
    s_idt[index] = (struct idt_entry){
        .base_0_15 = handler & 0xffffU,
        .base_16_31 = (handler >> 16U) & 0xffffU,
        .segment_selector = segment_index * sizeof(struct gdt_entry),
        .flags = IDT_FLAG_PRESENT | flags,
        .reserved = 0,
    };
}

void
idt_init()
{
    for (size_t interrupt = 0; interrupt < IDT_NUM_ENTRIES; interrupt++)
    {
        if (interrupt < ISR_NUM_DEFINED_HANDLERS)
        {
            idt_set_entry(
                interrupt,
                GDT_IDX_KCODE,
                (uint32_t)isr_0x00 + (interrupt * ISR_HANDLER_ALIGNMENT),
                IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
            );
        }
        else
        {
            idt_set_entry(
                interrupt,
                GDT_IDX_KCODE,
                (uint32_t)isr_unhandled_stub,
                IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
            );
        }
    }

    // idt_set_entry(
    //     0x20,
    //     GDT_IDX_KCODE,
    //     &idt_handle_interrupt_request_0x00,
    //     IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    // );
    //
    // idt_set_entry(
    //     0x21,
    //     GDT_IDX_KCODE,
    //     &idt_handle_interrupt_request_0x01,
    //     IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    // );

    // port_write_8(PORT_PIC_MASTER_COMMAND, 0x11U);
    // port_write_8(PORT_PIC_SLAVE_COMMAND, 0x11U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x20U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x28U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x04U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x02U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x01U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x01U);
    //
    // port_write_8(PORT_PIC_MASTER_DATA, 0x00U);
    // port_write_8(PORT_PIC_SLAVE_DATA, 0x00U);
}

void
idt_load()
{
    asm volatile("lidt %0" : : "m"(s_idtr) : "memory");
}

// void
// idt_activate()
// {
//     // TODO: Assumes there is one processor to listen to interrupts
//     asm volatile("sti");
// }

// void
// idt_deactivate()
// {
//     // TODO: Assumes there is one processor to stop listening to interrupts
//     asm volatile("cli");
// }

#define ITOA_DEFAULT_RADIX 10U
#define ITOA_MAX_STR_LEN   255U
#define ITOA_RADIX_MIN     2U
#define ITOA_RADIX_MAX     36U

const char *const ITOA_SEARCH_STR = "0123456789abcdefghijklmnopqrstuvwxyz";

void
write_uint(
    size_t integer,
    size_t radix,
    void (*handler)(struct vga *vga, const char *str)
)
{
    char str[ITOA_MAX_STR_LEN] = {0};
    size_t idx = 0;
    size_t value = integer;

    // Convert each digit to a character, from the least significant digit to
    // the most significant (i.e. in reverse order).
    do
    {
        str[idx] = ITOA_SEARCH_STR[value % radix];
        idx += 1;
        value /= radix; // this will truncate (discard) the fractional part
    } while (value != 0);

    // Ensure the string is terminated
    str[idx] = '\0';
    idx -= 1;

    // Flip string to the correct order
    size_t rev = 0;
    while (rev < idx)
    {
        char tmp = str[idx];
        str[idx] = str[rev];
        str[rev] = tmp;
        rev += 1;
        idx -= 1;
    }

    struct vga *vga = vga_get();
    handler(vga, str);
}

struct [[gnu::packed]] interrupt_frame
{
    // Data segment, manually pushed by us
    size_t ds;
    // Pushed with PUSHA, listed here in reverse order
    size_t edi, esi, ebp, kernel_esp, ebx, edx, ecx, eax;
    // Manually pushed by us
    size_t interrupt_number;
    // Manually pushed by us
    size_t error_code;
    // Pushed automatically by the CPU, listed here in reverse order
    size_t eip, cs, eflags, user_esp, ss;
};

[[gnu::cdecl]]
void
isr_handler(struct interrupt_frame *frame)
{
    // if (frame->interrupt_number == 0xd)
    // {
    //     asm volatile("hlt");
    // }

    struct vga *vga = vga_get();
    vga_print(vga, "INT ");
    write_uint(frame->interrupt_number, 10U, vga_println);
    // vga_print(vga, " -> EAX 0x");
    // write_uint(frame->eax, 16U, vga_print);
    // vga_print(vga, " EBX 0x");
    // write_uint(frame->ebx, 16U, vga_print);
    // vga_print(vga, " ECX 0x");
    // write_uint(frame->ecx, 16U, vga_print);
    // vga_print(vga, " EDX 0x");
    // write_uint(frame->edx, 16U, vga_println);
    // vga_print(vga, "    ESI 0x");
    // write_uint(frame->esi, 16U, vga_print);
    // vga_print(vga, " EDI 0x");
    // write_uint(frame->edi, 16U, vga_print);
    // vga_print(vga, " EBP 0x");
    // write_uint(frame->ebp, 16U, vga_print);
    // vga_print(vga, " ESP 0x");
    // write_uint(frame->kernel_esp, 16U, vga_println);
    // vga_print(vga, " EFLAGS 0x");
    // write_uint(frame->eflags, 16U, vga_println);
}
