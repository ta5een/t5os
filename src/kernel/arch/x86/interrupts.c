#include "interrupts.h"

#if defined(__x86_64__)
    #error "IDT and ISR headers not yet defined for x86_64"
#elif defined(__i386__)
    #include <kernel/arch/x86/32/interrupts/idt.h>
    #include <kernel/arch/x86/32/interrupts/isr.h>
#endif

void
interrupts_init()
{
#if defined(__x86_64__)
    #error "'interrupts_init' not yet defined for x86_64"
#elif defined(__i386__)
    ia32_idt_init();
    ia32_isr_init();
#endif
}
