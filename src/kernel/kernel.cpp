#include <drivers/vga.hpp>
#include <kernel/platform/platform.hpp>
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
    const char *arch_msg = nullptr;
    if (ARCH(I386))
    {
        arch_msg = "Architecture: i386";
    }
    else
    {
        arch_msg = "Architecture: unknown";
    }

    drivers::WRITER.clear_screen();
    drivers::WRITER.put_string("Welcome to WYOOS\n");
    drivers::WRITER.put_string(arch_msg);
    drivers::WRITER.new_line();

    // Infinite loop
    while (true) {}
}
