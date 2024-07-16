#include <kernel/arch/x86/devices/vga.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>
#include <kernel/arch/x86/isr.h>

#define ISR_HANDLER_ALIGNMENT      (16U)
#define ISR_EXCEPTION_VECTOR_START (0x00U)
#define ISR_INTERRUPT_VECTOR_START (0x20U)
#define ISR_NUM_EXCEPTIONS         (ISR_INTERRUPT_VECTOR_START)

/**
 * Points to the first ISR handler, defined in "isr.S".
 *
 * Since all handlers we define are aligned to ISR_HANDLER_ALIGNMENT bytes, we
 * can use the address of this handler to calculate the addresses of subsequent
 * ISRs. For example, to get the address of the 0x10th ISR, we can calculate it
 * like so:
 *
 *     isr_0x00 + (0x10 * ISR_HANDLER_ALIGNMENT)
 *
 * https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/05_InterruptHandling.md#an-example-stub
 */
extern i686_idt_handler_t i686_isr_0x00;

/**
 * All other ISRs will point to this stub (which for now will halt the OS).
 */
extern i686_idt_handler_t i686_isr_unhandled_stub;

void
i686_isr_init()
{
    // Initialize entries for exceptions with their respective handlers
    for (size_t exception = ISR_EXCEPTION_VECTOR_START;
         exception < ISR_INTERRUPT_VECTOR_START;
         exception++)
    {
        i686_idt_set_entry(
            exception,
            i686_isr_0x00 + (exception * ISR_HANDLER_ALIGNMENT),
            GDT_SELECTOR(GDT_IDX_KCODE),
            IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
        );
    }

    // Initialize the rest of the entries with the unhandled stub handler
    for (size_t interrupt = ISR_INTERRUPT_VECTOR_START;
         interrupt < IDT_NUM_ENTRIES;
         interrupt++)
    {
        i686_idt_set_entry(
            interrupt,
            i686_isr_unhandled_stub,
            GDT_SELECTOR(GDT_IDX_KCODE),
            IDT_FLAG_RING0 | IDT_FLAG_GATE_INT_32
        );
    }
}

// Temporary workaround until printf is implemented
static void
vga_print_uint(struct vga *vga, size_t integer, size_t radix);

[[gnu::cdecl]]
void
i686_isr_handler(struct i686_interrupt_frame *frame)
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
