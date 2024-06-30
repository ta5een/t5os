#include <drivers/keyboard.hpp>
#include <drivers/vga.hpp>
#include <kernel/arch/platform.hpp>
#include <kernel/arch/x86/gdt.hpp>
#include <kernel/arch/x86/interrupts.hpp>
#include <lib/integers.hpp>

using Multiboot = void *;
using CtorFunc = void (*)();

extern "C" CtorFunc start_ctors;
extern "C" CtorFunc end_ctors;

/**
 * Required run-time function for driving C++ constructors.
 */
extern "C" void call_ctors()
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (CtorFunc *ctor = &start_ctors; ctor != &end_ctors; ctor++)
    {
        (*ctor)();
    }
}

/**
 * Entry point of the kernel.
 */
extern "C" void _kmain(Multiboot /*multiboot*/, u32 /*magic*/)
{
    const char *arch_string{};
    if (ARCH(I386))
    {
        arch_string = "x86";
    }
    else
    {
        arch_string = "unknown";
    }

    drivers::WRITER.clear_screen();
    drivers::WRITER.put_string("t5os/1 (");
    drivers::WRITER.put_string(arch_string);
    drivers::WRITER.put_string(")\n");

    kernel::GlobalDescriptorTable gdt{};
    gdt.load();

    kernel::InterruptManager interrupts{&gdt};
    drivers::KeyboardDriver keyboard{&interrupts};
    interrupts.activate();

    // Infinite loop
    while (true) {}
}
