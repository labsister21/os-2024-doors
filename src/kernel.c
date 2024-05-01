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
#include "header/memory/paging.h"

// 2.2.5
void kernel_setup(void)
{
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    gdt_install_tss();
    set_tss_register();

    // Allocate first 4 MiB virtual memory
    paging_allocate_user_page_frame(&_paging_kernel_page_directory, (uint8_t *)0);

    // Write shell into memory
    struct FAT32DriverRequest request = {
        .buf = (uint8_t *)0,
        .name = "shell",
        .ext = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = 0x100000,
    };
    read(request);

    // Set TSS $esp pointer and jump into shell
    set_tss_kernel_current_stack();
    kernel_execute_user_program((uint8_t *)0);

    while (true)
        ;
}

// void kernel_setup(void)
// {
//     // load_gdt(&_gdt_gdtr);
//     // pic_remap();
//     // activate_keyboard_interrupt();
//     // init_keyboard_state();
//     // initialize_idt();
//     // framebuffer_clear();
//     // framebuffer_set_cursor(0, 0);
//     // initialize_filesystem_fat32();
//     // // uint8_t arr[] = "halo nama saya adalah nino nakano";
//     // // struct FAT32DriverRequest req={
//     // //   .buf = arr,
//     // //   .name="demooo",
//     // //   .ext="txt",
//     // //   .parent_cluster_number=ROOT_CLUSTER_NUMBER,
//     // //   .buffer_size = sizeof(arr) - 1
//     // // };
//     // // framebuffer_write(delete(req)+'0',0xF,0);
//     // // struct FAT32DriverRequest reqDelete={
//     // //   .name="ninoo",
//     // //   .ext="",
//     // //   .parent_cluster_number=ROOT_CLUSTER_NUMBER,
//     // //   .buffer_size = 0
//     // // };
//     // // framebuffer_write(write(reqDelete) + '0', 0xF, 0);
//     // // read(reqDelete);
//     // // uint16_t i;
//     // // uint8_t * tt = (uint8_t *)reqDelete.buf;
//     // // for (i = 0; i < reqDelete.buffer_size; i++) {
//     // //   char t = (char)tt[i];
//     // //   framebuffer_write(t, 0xF, 0);
//     // // }
//     // // print(frame_buffer.size, frame_buffer.size);
//     // while (true) {
//     //   get_keyboard_buffer(&c);
//     //   typing_keyboard();
//     // }
//     // keyboard_state_deactivate();
// }