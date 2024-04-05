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
    load_gdt(&_gdt_gdtr);
    pic_remap();
    activate_keyboard_interrupt();
    init_keyboard_state();
    initialize_idt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    uint8_t arr[] = "kontolllll";
    struct FAT32DriverRequest req={
      .buf = arr,
      .name="filebuat",
      .ext="",
      .parent_cluster_number=ROOT_CLUSTER_NUMBER,
      .buffer_size = CLUSTER_SIZE
    };
    framebuffer_write(write(req)+'0',0xF,0);
    struct FAT32DriverRequest reqDelete={
      .buf = NULL,
      .name="filebuat",
      .ext="",
      .parent_cluster_number=ROOT_CLUSTER_NUMBER,
      .buffer_size = 0
    };
    framebuffer_write(delete(reqDelete)+'0',0xF,0);
    // char * cc = "pepek";
    // memcpy(req.name, cc, sizeof(cc));
    // req.parent_cluster_number = 2;
    // req.buffer_size = 8000;
    // read(req);
    // uint16_t i;
    // uint8_t * tt = (uint8_t *)req.buf;
    // for (i = 0; i < req.buffer_size; i++) {
    //   char t = (char)tt[i];
    //   framebuffer_write(t, 0xF, 0);
    // }
    // print(frame_buffer.size, frame_buffer.size);
    while (true) {
      get_keyboard_buffer(&c);
      typing_keyboard();
    }
    keyboard_state_deactivate();
}