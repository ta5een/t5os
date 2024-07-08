#include <kernel/arch/x86/devices/serial.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>
#include <libcore/string_view.h>

typedef void(*multiboot_t);

static struct gdt s_gdt;
// static struct idt s_idt;

volatile uint16_t *VGA_BUFFER = (uint16_t *)0xb8000;

void putc(uint8_t byte)
{
    uint8_t color_code = ((uint8_t)0xfU) | (uint8_t)((uint8_t)0x0U << 4U);
    uint16_t screen_char = ((uint16_t)byte) | (uint16_t)(color_code << 8U);
    VGA_BUFFER[0] = screen_char;
}

void kmain(
    [[gnu::unused]] multiboot_t /*multiboot*/,
    [[gnu::unused]] uint32_t /*magic*/
)
{
    enum serial_init_response response = serial_init(SERIAL_COM1_BASE);
    if (response == SERIAL_INIT_SUCCESS)
    {
        // TODO: Do something useful here...
        putc('Y');
    }
    else
    {
        // TODO: Do something useful here...
        putc('N');
    }

    string_view_t view;
    LIBCORE_RESIZE_STRING_VIEW(view, "WRITING TO SERIAL PORT\n");
    serial_write(SERIAL_COM1_BASE, view);

    LIBCORE_RESIZE_STRING_VIEW(view, "ANOTHER STRING TEST\n");
    serial_write(SERIAL_COM1_BASE, view);

    LIBCORE_RESIZE_STRING_VIEW(view, "ONE MORE STRING TEST\n");
    serial_write(SERIAL_COM1_BASE, view);

    // Load the GDT for the BSP
    gdt_init(&s_gdt);
    gdt_load(&s_gdt);

    // Load the IDT for the BSP
    // idt_init(&s_idt, GDT_IDX_KCODE);
    // idt_load(&s_idt);
    // idt_activate(&s_idt);

    // Idle loop
    while (true)
    {
#if defined(__i386__) || defined(__x86_64__)
        // Use the hlt instruction to put the CPU in a low-power state when
        // idle. This reduces CPU usage and power consumption compared to a
        // busy-wait loop.
        asm volatile("hlt");
#else
    #warning "Idle loop not defined for current architecture"
#endif
    }
}
