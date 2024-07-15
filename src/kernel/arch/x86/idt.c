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

typedef void(isr_handler_t)(void);

extern isr_handler_t isr_0x00;
extern isr_handler_t isr_0x01;
extern isr_handler_t isr_0x02;
extern isr_handler_t isr_0x03;
extern isr_handler_t isr_0x04;
extern isr_handler_t isr_0x05;
extern isr_handler_t isr_0x06;
extern isr_handler_t isr_0x07;
extern isr_handler_t isr_0x08;
extern isr_handler_t isr_0x09;
extern isr_handler_t isr_0x0a;
extern isr_handler_t isr_0x0b;
extern isr_handler_t isr_0x0c;
extern isr_handler_t isr_0x0d;
extern isr_handler_t isr_0x0e;
extern isr_handler_t isr_0x0f;
extern isr_handler_t isr_0x10;
extern isr_handler_t isr_0x11;
extern isr_handler_t isr_0x12;
extern isr_handler_t isr_0x13;
extern isr_handler_t isr_0x14;
extern isr_handler_t isr_0x15;
extern isr_handler_t isr_0x16;
extern isr_handler_t isr_0x17;
extern isr_handler_t isr_0x18;
extern isr_handler_t isr_0x19;
extern isr_handler_t isr_0x1a;
extern isr_handler_t isr_0x1b;
extern isr_handler_t isr_0x1c;
extern isr_handler_t isr_0x1d;
extern isr_handler_t isr_0x1e;
extern isr_handler_t isr_0x1f;

/**
 * Points to the first ISR handler, defined in "isr.S".
 *
 * The type of `isr_0x00` is not important, we only care about the address it
 * occupies. Since all handlers we currently define are aligned to 16 bytes, we
 * can use this address to calculate the addresses of subsequent ISRs. For
 * example, to get the address of the 0x10th ISR, we can calculate it like so:
 *
 *     isr_0x00 + (0x10 * ISR_HANDLER_ALIGNMENT)
 *
 * https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/05_InterruptHandling.md#an-example-stub
 */
// extern uint8_t isr_0x00[];

/**
 * All other ISRs will point to this stub (which for now will halt the OS).
 */
extern isr_handler_t isr_unhandled_stub;

static struct idt_entry s_idt[IDT_NUM_ENTRIES];

static const struct descriptor_table_register s_idtr = {
    .limit = sizeof(s_idt) - 1,
    .base = (void *)s_idt,
};

void
idt_set_entry(
    size_t index,
    size_t segment_selector,
    uint32_t handler,
    enum idt_flag flags
)
{
    s_idt[index] = (struct idt_entry){
        .base_0_15 = (handler) & 0xffffU,
        .base_16_31 = (handler >> 16U) & 0xffffU,
        .segment_selector = segment_selector,
        .flags = IDT_FLAG_PRESENT | flags,
        .reserved = 0,
    };
}

void
idt_init()
{
    // for (size_t interrupt = 0; interrupt < IDT_NUM_ENTRIES; interrupt++)
    // {
    //     if (interrupt < ISR_NUM_DEFINED_HANDLERS)
    //     {
    //         idt_set_entry(
    //             interrupt,
    //             0x10,
    //             (uint32_t)isr_0x00 + (interrupt * ISR_HANDLER_ALIGNMENT),
    //             IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    //         );
    //     }
    //     else
    //     {
    //         idt_set_entry(
    //             interrupt,
    //             0x10,
    //             (uint32_t)isr_unhandled_stub,
    //             IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
    //         );
    //     }
    // }

    for (size_t interrupt = 0; interrupt < IDT_NUM_ENTRIES; interrupt++)
    {
        idt_set_entry(interrupt, 0x10, (uint32_t)isr_unhandled_stub, 0xe);
    }

    idt_set_entry(0x00, 0x10, (uint32_t)isr_0x00, 0xe);
    idt_set_entry(0x01, 0x10, (uint32_t)isr_0x01, 0xe);
    idt_set_entry(0x02, 0x10, (uint32_t)isr_0x02, 0xe);
    idt_set_entry(0x03, 0x10, (uint32_t)isr_0x03, 0xe);
    idt_set_entry(0x04, 0x10, (uint32_t)isr_0x04, 0xe);
    idt_set_entry(0x05, 0x10, (uint32_t)isr_0x05, 0xe);
    idt_set_entry(0x06, 0x10, (uint32_t)isr_0x06, 0xe);
    idt_set_entry(0x07, 0x10, (uint32_t)isr_0x07, 0xe);
    idt_set_entry(0x08, 0x10, (uint32_t)isr_0x08, 0xe);
    idt_set_entry(0x09, 0x10, (uint32_t)isr_0x09, 0xe);
    idt_set_entry(0x0a, 0x10, (uint32_t)isr_0x0a, 0xe);
    idt_set_entry(0x0b, 0x10, (uint32_t)isr_0x0b, 0xe);
    idt_set_entry(0x0c, 0x10, (uint32_t)isr_0x0c, 0xe);
    idt_set_entry(0x0d, 0x10, (uint32_t)isr_0x0d, 0xe);
    idt_set_entry(0x0e, 0x10, (uint32_t)isr_0x0e, 0xe);
    idt_set_entry(0x0f, 0x10, (uint32_t)isr_0x0f, 0xe);
    idt_set_entry(0x10, 0x10, (uint32_t)isr_0x10, 0xe);
    idt_set_entry(0x11, 0x10, (uint32_t)isr_0x11, 0xe);
    idt_set_entry(0x12, 0x10, (uint32_t)isr_0x12, 0xe);
    idt_set_entry(0x13, 0x10, (uint32_t)isr_0x13, 0xe);
    idt_set_entry(0x14, 0x10, (uint32_t)isr_0x14, 0xe);
    idt_set_entry(0x15, 0x10, (uint32_t)isr_0x15, 0xe);
    idt_set_entry(0x16, 0x10, (uint32_t)isr_0x16, 0xe);
    idt_set_entry(0x17, 0x10, (uint32_t)isr_0x17, 0xe);
    idt_set_entry(0x18, 0x10, (uint32_t)isr_0x18, 0xe);
    idt_set_entry(0x19, 0x10, (uint32_t)isr_0x19, 0xe);
    idt_set_entry(0x1a, 0x10, (uint32_t)isr_0x1a, 0xe);
    idt_set_entry(0x1b, 0x10, (uint32_t)isr_0x1b, 0xe);
    idt_set_entry(0x1c, 0x10, (uint32_t)isr_0x1c, 0xe);
    idt_set_entry(0x1d, 0x10, (uint32_t)isr_0x1d, 0xe);
    idt_set_entry(0x1e, 0x10, (uint32_t)isr_0x1e, 0xe);
    idt_set_entry(0x1f, 0x10, (uint32_t)isr_0x1f, 0xe);

    // TODO: Write to the PIC to remap the IRQs
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

// Temporary workaround until printf is implemented
void
vga_print_uint(struct vga *vga, size_t integer, size_t radix);

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
    // Print the interrupt number for now
    struct vga *vga = vga_get();
    vga_print(vga, "INT ");
    // Until printf is implemented, this will do for now
    vga_print_uint(vga, frame->interrupt_number, 10U);
}

#define ITOA_DEFAULT_RADIX 10U
#define ITOA_MAX_STR_LEN   255U
#define ITOA_RADIX_MIN     2U
#define ITOA_RADIX_MAX     36U

const char *const ITOA_SEARCH_STR = "0123456789abcdefghijklmnopqrstuvwxyz";

void
vga_print_uint(struct vga *vga, size_t integer, size_t radix)
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

    vga_println(vga, str);
}
