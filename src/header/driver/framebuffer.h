#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define FRAMEBUFFER_MEMORY_OFFSET ((uint8_t *)0xB8000)
#define CURSOR_PORT_CMD 0x03D4
#define CURSOR_PORT_DATA 0x03D5

#define BUFFER_HEIGHT_VIEW 25
#define BUFFER_WIDTH_VIEW 79
#define BUFFER_MAX_HEIGHT 4096

struct LineBuffer {
    uint8_t size;
    uint8_t line_buf[BUFFER_WIDTH_VIEW];
} __attribute((packed));

struct FrameBuffer {
    int size;
    struct LineBuffer buffer[BUFFER_MAX_HEIGHT];
}__attribute((packed));

extern struct FrameBuffer frame_buffer;

extern int cursor_row, cursor_col, frame_row_pointer;
extern char c;

void init_frame_buffer();

/**
 * Terminal framebuffer
 * Resolution: 80x25
 * Starting at FRAMEBUFFER_MEMORY_OFFSET,
 * - Even number memory: Character, 8-bit
 * - Odd number memory:  Character color lower 4-bit, Background color upper 4-bit
 */

/**
 * Set framebuffer character and color with corresponding parameter values.
 * More details: https://en.wikipedia.org/wiki/BIOS_color_attributes
 *
 * @param row Vertical location (index start 0)
 * @param col Horizontal location (index start 0)
 * @param c   Character
 * @param fg  Foreground / Character color
 * @param bg  Background color
 */
void framebuffer_write(char c, uint8_t fg, uint8_t bg);

/**
 * Set cursor to specified location. Row and column starts from 0
 *
 * @param r row
 * @param c column
 */
void framebuffer_set_cursor(uint8_t r, uint8_t c);

/**
 * Set all cell in framebuffer character to 0x00 (empty character)
 * and color to 0x07 (gray character & black background)
 * Extra note: It's allowed to use different color palette for this
 *
 */
void new_frame_buffer_view(uint8_t fg, uint16_t bg, bool change);

void framebuffer_clear(void);

void init_keyboard_state(void);

void print(int row, int col);

void framebuffer_erase(int *row, int * col);

void typing_keyboard();

#endif