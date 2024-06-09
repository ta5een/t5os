typedef void *multiboot_t;
typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

unsigned short *VGA_MEMORY = (unsigned short *)0xb8000;

void kwrite(const char *str) {
  for (int i = 0; str[i] != '\0'; ++i) {
    // Don't overwrite the high bits to keep default forground and background
    VGA_MEMORY[i] = (VGA_MEMORY[i] & 0xFF00) | str[i];
  }
}

extern "C" void call_ctors() {
  for (constructor *ctor = &start_ctors; ctor != &end_ctors; ++ctor) {
    (*ctor)();
  }
}

extern "C" void kmain(multiboot_t multiboot, unsigned int magic) {
  kwrite("Hello, world!\n");
  while (true) {
    // Infinite loop
  }
}
