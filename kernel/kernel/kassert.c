#include "kernel/kassert.h"
#include "arch/x86_shared/devices/vga.h"
#include "kernel/kpanic.h"

void
kernel_on_assert(const char *msg, const char *location, const char *function)
{
    // TODO: Write to VGA and serial port...
    struct vga *vga = vga_get();
    vga_println(vga, "*** ASSERTION FAILURE ***");
    vga_print(vga, "-> ");
    vga_println(vga, msg);
    vga_print(vga, "-> at ");
    vga_println(vga, location);
    vga_print(vga, "-> in ");
    vga_println(vga, function);

    KPANIC("Cannot continue due to assertion failure");
}
