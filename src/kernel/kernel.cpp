#include <kernel/arch/platform.hpp>
#include <lib/integers.hpp>

using Multiboot = void *;
using Constructor = void (*)();

enum class VgaColor : u8
{
    Black = 0x0,
    Blue = 0x1,
    Green = 0x2,
    Cyan = 0x3,
    Red = 0x4,
    Magenta = 0x5,
    Brown = 0x6,
    LightGrey = 0x7,
    DarkGrey = 0x8,
    LightBlue = 0x9,
    LightGreen = 0xA,
    LightCyan = 0xB,
    LightRed = 0xC,
    LightMegenta = 0xD,
    LightBrown = 0xE,
    White = 0xF,
};

u16 *const VGA_MEMORY = (u16 *)0xB8000;
const u8 VGA_SCREEN_COLOR = (u8)VgaColor::White | ((u8)VgaColor::Black << 4);

extern "C" Constructor start_ctors;
extern "C" Constructor end_ctors;

/**
 * Output a white-on-black character to the VGA memory buffer.
 */
void vga_write_to_buffer(const char *str)
{
    for (u8 i = 0; str[i] != '\0'; i++)
    {
        // Set the highest bits to the default foreground and background
        VGA_MEMORY[i] = (u16)str[i] | (u16)(VGA_SCREEN_COLOR) << 8;
    }
}

/**
 * Required run-time function for driving C++ constructors.
 */
extern "C" void call_ctors()
{
    for (Constructor *ctor = &start_ctors; ctor != &end_ctors; ctor++)
    {
        (*ctor)();
    }
}

/**
 * Entry point of the kernel.
 */
extern "C" void _kmain(Multiboot /* multiboot */, u32 /* magic */)
{
    const char *welcome_msg;
    if (KERNEL_ARCH_IS_I386())
    {
        welcome_msg = "Welcome to WYOOS (arch=i386)";
    }

    vga_write_to_buffer(welcome_msg);

    // Infinite loop
    while (true) {}
}
