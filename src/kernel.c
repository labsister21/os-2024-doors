#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/text/framebuffer.h"

void kernel_setup(void)
{
    // load_gdt(&_gdt_gdtr);
    framebuffer_clear();
    framebuffer_write(3, 9, 'H', 0, 0xF);
    framebuffer_write(3, 10, 'a', 0, 0xF);
    framebuffer_write(3, 11, 'i', 0, 0xF);
    framebuffer_write(3, 12, '!', 0, 0xF);
    framebuffer_set_cursor(3, 12);
    while (true)
        ;
}
