#pragma once

#include <stddef.h>

/**
 * The contents of the stack when an interrupt is ready to be handled in C.
 */
struct [[gnu::packed]] x86_32_iframe
{
    /**
     * The data segment selector at the time the interrupt was triggered,
     * manually pushed by us.
     */
    size_t ds;
    /**
     * Register values pushed with PUSHA, listed here in reverse order.
     */
    size_t edi, esi, ebp, kernel_esp, ebx, edx, ecx, eax;
    /**
     * The interrupt vector number, manually pushed by us.
     */
    size_t interrupt_number;
    /**
     * The error code for this interrupt.
     *
     * This value may be pushed automatically by the CPU if the interrupt
     * originally contained an error code. Otherwise, a default value of `0`
     * would be manually pushed by us.
     */
    size_t error_code;
    /**
     * Information provided by the CPU for all interrupts.
     */
    size_t eip, cs, eflags, user_esp, ss;
};
