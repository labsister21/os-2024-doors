#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/cpu/interrupt.h"
// #include <stdio.h>

void kernel_setup(void)
{
  load_gdt(&_gdt_gdtr);
  pic_remap();
  initialize_idt();
  activate_keyboard_interrupt();
  framebuffer_clear();
  framebuffer_set_cursor(0, 0);
  keyboard_state_activate();
  int col = 0;
  int row = 0;
  while (true)
  {
    char c;
    get_keyboard_buffer(&c);
    if (c){
      // printf("char: %d\n", (int) c);
      if (c != '\b') {
        framebuffer_write(row, col, c, 0xF, 0);
        col++;
        row += col/80;
        col %= 80;
        framebuffer_set_cursor(row, col);
      } else if (col > 0 || row > 0) {
        framebuffer_erase(&row, &col);
        framebuffer_set_cursor(row, col);
      }
    }
      
  }
}
