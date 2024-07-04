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
#if ARCH(X86_32)
    const char *arch_string = "x86";
#else
#    error "Unhandled case for current architecture"
#endif

    const auto &vga = drivers::VgaWriter::instance();
    vga.clear_screen();
    vga.clear_screen();
    vga.put_string("t5os/1 (");
    vga.put_string(arch_string);
    vga.put_string(")\n");

    constexpr usize LIMIT_4GiB = 0xfffff;

    kernel::GlobalDescriptorTable gdt{};
    // Null
    gdt.write_entry(kernel::GlobalDescriptorTable::NULL_IDX, {});
    // Unused
    gdt.write_entry(kernel::GlobalDescriptorTable::UNUSED_IDX, {});
    // Kernel code
    gdt.write_entry(
        kernel::GlobalDescriptorTable::KERNEL_CS_IDX,
        kernel::SegmentDescriptor{
            0,
            LIMIT_4GiB,
            kernel::SegmentDescriptor::ACCESS_RING0 |
                kernel::SegmentDescriptor::ACCESS_PRESENT |
                kernel::SegmentDescriptor::ACCESS_USER_SEGMENT |
                kernel::SegmentDescriptor::ACCESS_EXECUTABLE |
                kernel::SegmentDescriptor::ACCESS_WRITEABLE,
            kernel::SegmentDescriptor::FLAG_GRANULARITY |
                kernel::SegmentDescriptor::FLAG_DEFAULT_SIZE

        }
    );
    // Kernel data
    gdt.write_entry(
        kernel::GlobalDescriptorTable::KERNEL_DS_IDX,
        kernel::SegmentDescriptor{
            0,
            LIMIT_4GiB,
            kernel::SegmentDescriptor::ACCESS_RING0 |
                kernel::SegmentDescriptor::ACCESS_PRESENT |
                kernel::SegmentDescriptor::ACCESS_USER_SEGMENT |
                kernel::SegmentDescriptor::ACCESS_WRITEABLE,
            kernel::SegmentDescriptor::FLAG_GRANULARITY |
                kernel::SegmentDescriptor::FLAG_DEFAULT_SIZE
        }
    );

    gdt.load();

    kernel::InterruptManager interrupts{&gdt};
    drivers::KeyboardDriver keyboard{&interrupts};
    interrupts.activate();

    // Idle loop
    while (true)
    {
#if ARCH(X86_32) || ARCH(X86_64)
        // Use the hlt instruction to put the CPU in a low-power state when
        // idle. This reduces CPU usage and power consumption compared to a
        // busy-wait loop.
        // NOLINTNEXTLINE(hicpp-no-assembler)
        asm volatile("hlt");
#endif
    }
}
