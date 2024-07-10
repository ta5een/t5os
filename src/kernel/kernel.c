#include <kernel/arch/x86/devices/serial.h>
#include <kernel/arch/x86/devices/vga.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>
#include <libcore/string_view.h>

#define VGA_BUFFER_ADDRESS ((volatile vga_char_t *)0xb8000U)

typedef void(*multiboot_t);

// Variables that will be available for the lifetime of the kernel
static volatile vga_char_t *s_vga_buffer = VGA_BUFFER_ADDRESS;
static struct gdt s_gdt;
// static struct idt s_idt;

void
kmain(multiboot_t /*multiboot*/, uint32_t /*magic*/)
{
    struct vga *vga = vga_get();
    vga_init(vga, s_vga_buffer);
    vga_clear_screen(vga);
    vga_println(vga, "Welcome to t5os");

    if (serial_init(SERIAL_COM1_BASE) != SERIAL_INIT_SUCCESS)
    {
        // TODO: Do something useful here...
        vga_println(vga, "[ WARN ] Serial connection could not be initialized");
    }

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
