#include <kernel/arch/x86/devices/serial.h>
#include <kernel/arch/x86/devices/vga.h>
#include <kernel/arch/x86/gdt.h>
#include <kernel/arch/x86/idt.h>

#define VGA_BUFFER_ADDRESS ((volatile vga_char_t *)0xb8000U)

typedef void(*multiboot_t);

// Variables that will be available for the lifetime of the kernel
static volatile vga_char_t *s_vga_buffer = VGA_BUFFER_ADDRESS;

void
kmain(multiboot_t /*multiboot*/, uint32_t /*magic*/)
{
    struct vga *vga = vga_get();
    vga_init(vga, s_vga_buffer);
    vga_clear_screen(vga);
    vga_println(vga, "t5os v0.0.1");

    vga_print(vga, "Clearing interrupts...");
    asm volatile("cli");
    vga_println(vga, " [DONE]");

    if (serial_init(SERIAL_COM1_BASE) != SERIAL_INIT_SUCCESS)
    {
        // TODO: Do something useful here...
        vga_println(vga, "[ WARN ] Serial connection could not be initialized");
    }

    // Load the GDT for the BSP
    gdt_init();
    gdt_load();

    // Load the IDT for the BSP
    idt_init();
    idt_load();
    // TODO: Activate all interrupts with STI once IRQs are implemented
    // idt_activate();

    // Test software interrupts
    vga_print(vga, "Triggering INT 0x02...");
    asm volatile("int $0x02");
    vga_println(vga, " [DONE]");

    vga_print(vga, "Triggering INT 0x03...");
    asm volatile("int $0x03");
    vga_println(vga, " [DONE]");

    vga_print(vga, "Triggering INT 0x04...");
    asm volatile("int $0x04");
    vga_println(vga, " [DONE]");

    // Idle loop
    // TODO: Move this to generic CPU interface
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
