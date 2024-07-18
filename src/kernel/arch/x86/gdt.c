#include "gdt.h"

#include <kernel/arch/x86/descriptor_table.h>
#include <stddef.h>

#if defined(__x86_64__)
    #error "GDT header not yet defined for x86_64"
#elif defined(__i386__)
    #include <kernel/arch/x86/32/gdt.h>
#endif

void
gdt_init()
{
#if defined(__x86_64__)
    #error "'gdt_init' not yet defined for x86_64"
#elif defined(__i386__)
    ia32_gdt_init();
#endif
}
