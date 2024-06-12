using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using s8 = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;

using multiboot_t = void *;
using constructor_t = void (*)();

enum vga_color {
  VGA_COLOR_BLACK = 0x0,
  VGA_COLOR_BLUE = 0x1,
  VGA_COLOR_GREEN = 0x2,
  VGA_COLOR_CYAN = 0x3,
  VGA_COLOR_RED = 0x4,
  VGA_COLOR_MAGENTA = 0x5,
  VGA_COLOR_BROWN = 0x6,
  VGA_COLOR_LIGHT_GREY = 0x7,
  VGA_COLOR_DARK_GREY = 0x8,
  VGA_COLOR_LIGHT_BLUE = 0x9,
  VGA_COLOR_LIGHT_GREEN = 0xA,
  VGA_COLOR_LIGHT_CYAN = 0xB,
  VGA_COLOR_LIGHT_RED = 0xC,
  VGA_COLOR_LIGHT_MAGENTA = 0xD,
  VGA_COLOR_LIGHT_BROWN = 0xE,
  VGA_COLOR_WHITE = 0xF,
};

u16 *const VGA_MEMORY = (u16 *)0xB8000;
u8 const VGA_SCREEN_COLOR = VGA_COLOR_WHITE | (VGA_COLOR_BLACK << 4);

extern "C" constructor_t start_ctors;
extern "C" constructor_t end_ctors;

void vga_write_to_buffer(const char *str) {
  for (u8 i = 0; str[i] != '\0'; i++) {
    // Set the highest bits to the default foreground and background
    VGA_MEMORY[i] = (u16)str[i] | (u16)(VGA_SCREEN_COLOR) << 8;
  }
}

extern "C" void call_ctors() {
  for (constructor_t *ctor = &start_ctors; ctor != &end_ctors; ctor++) {
    (*ctor)();
  }
}

extern "C" void kmain(multiboot_t multiboot, u32 magic) {
  vga_write_to_buffer("Hello, world!");
  while (true) {
    // Infinite loop
  }
}
