#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/text/framebuffer.h"
#include "header/cpu/idt.h"
#include "header/interrupt/interrupt.h"

void kernel_setup(void)
{
  // load_gdt(&_gdt_gdtr);
  // framebuffer_clear();
  // framebuffer_write(3, 9, 'H', 0, 0xF);
  // framebuffer_write(3, 10, 'a', 0, 0xF);
  // framebuffer_write(3, 11, 'i', 0, 0xF);
  // framebuffer_write(3, 12, '!', 0, 0xF);
  // framebuffer_set_cursor(3, 12);
  load_gdt(&_gdt_gdtr);
  pic_remap();
  initialize_idt();
  framebuffer_clear();
  framebuffer_set_cursor(0, 0);
  __asm__("int $0x4");
  while (true)
    ;
}
