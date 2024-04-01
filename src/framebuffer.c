#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/driver/framebuffer.h"
#include "header/stdlib/string.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"

void framebuffer_set_cursor(uint8_t r, uint8_t c)
{
    // TODO : Implement
    uint16_t pos = 80 * r + c;
    out(CURSOR_PORT_CMD, 0x0F);
    out(CURSOR_PORT_DATA, (uint8_t)(pos & 0xFF));
    out(CURSOR_PORT_CMD, 0x0E);
    out(CURSOR_PORT_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg)
{
    // TODO : Implement
    uint8_t attr = (bg << 4) | (fg);
    FRAMEBUFFER_MEMORY_OFFSET[row * 160 + col * 2] = c;
    FRAMEBUFFER_MEMORY_OFFSET[row * 160 + col * 2 + 1] = attr;
}

void framebuffer_clear(void)
{
    // TODO : Implement
    size_t n = 2 * 80 * 25;
    for (size_t i = 0; i < n; i += 2)
    {
        FRAMEBUFFER_MEMORY_OFFSET[i] = 0x00;
        FRAMEBUFFER_MEMORY_OFFSET[i + 1] = 0x07;
    }
}

void framebuffer_erase(int *row, int *col) {
    int pos = 160*(*row) + ((*col)%80-1)*2;
    FRAMEBUFFER_MEMORY_OFFSET[pos] = 0x00;
    FRAMEBUFFER_MEMORY_OFFSET[pos+1] = 0x07;
    if (*col > 0 || *row > 0) {
        (*col) --;
        if (*col < 0) {
            (*row) --;
            *col = 79;
        }
    }
}