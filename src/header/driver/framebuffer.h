#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define FRAMEBUFFER_MEMORY_OFFSET ((uint8_t *)0xC00B8000)
#define CURSOR_PORT_CMD 0x03D4
#define CURSOR_PORT_DATA 0x03D5

#define BUFFER_HEIGHT_VIEW 25
#define BUFFER_WIDTH_VIEW 79
#define BUFFER_MAX_HEIGHT 4096

struct LineBuffer
{
    uint8_t size;
    uint8_t line_buf[BUFFER_WIDTH_VIEW];
    uint8_t color_buf[BUFFER_WIDTH_VIEW];
} __attribute((packed));

struct FrameBuffer
{
    int size;
    struct LineBuffer buffer[BUFFER_MAX_HEIGHT];
} __attribute((packed));

extern struct FrameBuffer frame_buffer;

extern int cursor_row, cursor_col, frame_row_pointer;

void init_frame_buffer();

void put_char_color(char c, uint32_t color);

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
void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg);

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
void new_frame_buffer_view(bool change);

void framebuffer_clear(void);

void init_keyboard_state(void);

void framebuffer_erase(int *row, int *col);

void enable_cursor(uint8_t start, uint8_t end);

void disable_cursor();

void clear_screen();

void put_char(char c, uint8_t fg, uint8_t bg);

void puts(const char *str, uint32_t cnt, uint32_t color);

void handle_new_char(char c, uint8_t fg, uint8_t bg);

#endif