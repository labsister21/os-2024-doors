#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/cpu/interrupt.h"
#include "header/driver/disk.h"

void kernel_setup(void)
{
  // JANGAN HAPUS YANG INI DAHULU //
  // load_gdt(&_gdt_gdtr);
  // pic_remap();
  // initialize_idt();
  // activate_keyboard_interrupt();
  // framebuffer_clear();
  // framebuffer_set_cursor(0, 0);
  // keyboard_state_activate();
  // int col = 0;
  // int row = 0;
  // init_new_line_table();
  // while (true)
  // {
  //   char c;
  //   get_keyboard_buffer(&c);
  //   if (c){
  //     if (c != '\b') {
  //       if (c != '\n') {
  //         framebuffer_write(row, col, c, 0xF, 0);
  //         col++;
  //         row += col/80;
  //         col %= 80;
  //         framebuffer_set_cursor(row, col);
  //       } else {
  //         if (col != 79) {
  //           new_line_table.table[new_line_table.size].row = (uint8_t) row;
  //           new_line_table.table[new_line_table.size].col = (uint8_t) col;
  //           new_line_table.size++;
  //         }
  //         col = 0;
  //         row ++;
  //         framebuffer_set_cursor(row, col);
  //       }
  //     } else if (col > 0 || row > 0) {
  //       if (col == 0 && new_line_table.size > 0) {
  //         row--;
  //         col = new_line_table.table[new_line_table.size-1].col;
  //         new_line_table.size--;
  //         framebuffer_set_cursor(row, col);
  //       } else {
  //         framebuffer_erase(&row, &col);
  //         framebuffer_set_cursor(row, col);
  //       }
  //     }
  //   }

  // }

  // load_gdt(&_gdt_gdtr);
  // pic_remap();
  // initialize_idt();
  // activate_keyboard_interrupt();
  // framebuffer_clear();
  // framebuffer_set_cursor(0, 0);
  // int col = 0;
  // keyboard_state_activate();
  // while (true)
  // {
  //   char c;
  //   get_keyboard_buffer(&c);
  //   if (c)
  //   {
  //     framebuffer_write(0, col++, c, 0xF, 0);
  //     framebuffer_set_cursor(0, col);
  //   }
  // }

  load_gdt(&_gdt_gdtr);
  pic_remap();
  activate_keyboard_interrupt();
  initialize_idt();
  framebuffer_clear();
  framebuffer_set_cursor(0, 0);

  struct BlockBuffer b;
  for (int i = 0; i < 512; i++)
    b.buf[i] = i % 16;
  write_blocks(&b, 17, 1);
  while (true)
    ;
}
