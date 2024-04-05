#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/stdlib/string.h"
#include "header/cpu/interrupt.h"
#include "header/driver/disk.h"
#include "header/filesystem/fat32.h"

void kernel_setup(void)
{
    // JANGAN HAPUS YANG INI DAHULU //
    //   load_gdt(&_gdt_gdtr);
    //   pic_remap();
    //   initialize_idt();
    //   activate_keyboard_interrupt();
    //   framebuffer_clear();
    //   framebuffer_set_cursor(0, 0);
    //   keyboard_state_activate();
    //   int col = 0;
    //   int row = 0;
    //   init_new_line_table();
    //   while (true)
    //   {
    //     char c;
    //     get_keyboard_buffer(&c);
    //     typing_keyboard(&row, &col, c);
    //   }
    //   keyboard_state_deactivate();
    load_gdt(&_gdt_gdtr);
    pic_remap();
    activate_keyboard_interrupt();
    initialize_idt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    // struct BlockBuffer b;
    // for (int i = 0; i < 512; i++)
    //     b.buf[i] = i % 16;
    // write_blocks(&b, 17, 1);
    initialize_filesystem_fat32();
    struct FAT32DriverRequest req;
    char * c = "kano";
    memcpy(req.name, c, sizeof(c));
    req.parent_cluster_number = 2;
    req.buffer_size = 10000;
    read(req);
    int idx = 0;
    uint8_t * tt = (uint8_t *)req.buf;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            char t = (char)tt[idx];
            framebuffer_write(i, j, t, 0xF, 0);
            idx++;
        }
    }
    // framebuffer_write(1, 3, res + '0', 0xF, 0);
    while (true)
        ;
}
