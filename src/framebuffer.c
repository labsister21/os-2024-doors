#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/text/framebuffer.h"
#include "header/stdlib/string.h"
#include "header/cpu/portio.h"

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    int a = r;
    int b = c;
    c = a + b;
    // TODO : Implement
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    // TODO : Implement
    uint8_t attr = (bg << 4) | (fg);
    FRAMEBUFFER_MEMORY_OFFSET[row * 80 + col * 2] = c;
    FRAMEBUFFER_MEMORY_OFFSET[row * 80 + col * 2 + 1] = attr;
}

void framebuffer_clear(void) {
    // TODO : Implement
}