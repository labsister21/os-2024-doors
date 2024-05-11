#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/driver/framebuffer.h"
#include "header/stdlib/string.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"
#include "header/driver/keyboard.h"

struct FrameBuffer frame_buffer;

int cursor_row, cursor_col, frame_row_pointer;

bool is_cursor_viewable = true;

bool get_is_cursor_viewable()
{
    return is_cursor_viewable;
}

void init_frame_buffer()
{
    frame_buffer.size = 1;
    is_cursor_viewable = true;
    for (uint16_t i = 0; i < BUFFER_MAX_HEIGHT; i++)
    {
        frame_buffer.buffer[i].size = 0;
    }
}

void enable_cursor(uint8_t start, uint8_t end)
{
    out(0x3D4, 0x0A);
    out(0x3D5, (in(0x3D5) & 0xC0) | start);

    out(0x3D4, 0x0B);
    out(0x3D5, (in(0x3D5) & 0xE0) | end);
}

void disable_cursor()
{
    out(0x3D4, 0x0A);
    out(0x3D5, 0x20);
}

void clear_screen()
{
    framebuffer_clear();
    frame_row_pointer = 0;
    cursor_col = 0;
    cursor_row = 0;
    is_cursor_viewable = true;
    framebuffer_set_cursor(0, 0);
    for (int i = 0; i < BUFFER_MAX_HEIGHT; i++)
    {
        frame_buffer.buffer[i].size = 0;
        memset(frame_buffer.buffer[i].line_buf, 0, 79);
        memset(frame_buffer.buffer[i].color_buf, 0, 79);
    }
    frame_buffer.size = 1;
}

void set_cursor_col(uint32_t col)
{
    cursor_col = col;
    framebuffer_set_cursor(cursor_row, cursor_col);
}

void framebuffer_set_cursor(uint8_t r, uint8_t c)
{
    if (!is_cursor_viewable) 
    {
        disable_cursor();
        return;
    }
    enable_cursor(14, 15);
    uint16_t pos = 80 * r + c;
    out(CURSOR_PORT_CMD, 0x0F);
    out(CURSOR_PORT_DATA, (uint8_t)(pos & 0xFF));
    out(CURSOR_PORT_CMD, 0x0E);
    out(CURSOR_PORT_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

void move_screen(char c)
{
    switch (c)
    {
    case ARROW_UP:
        /* code */
        if (frame_row_pointer > 0)
        {
            frame_row_pointer--;
        }
        break;
    case ARROW_DOWN:
        if (frame_row_pointer + BUFFER_HEIGHT_VIEW < frame_buffer.size)
        {
            frame_row_pointer++;
        }
        break;
    default:
        break;
    }
    if (c == ARROW_UP || c == ARROW_DOWN)
    {
        // check if cursor is viewable
        if (frame_buffer.size > frame_row_pointer && frame_buffer.size <= frame_row_pointer + BUFFER_HEIGHT_VIEW)
        {
            is_cursor_viewable = true;
        }
        else
        {
            is_cursor_viewable = false;
        }

        uint32_t row = (uint8_t)(frame_buffer.size - frame_row_pointer);
        if (row > 25)
        {
            row = 25;
        }
        framebuffer_clear();
        for (uint32_t i = 0; i < row; i++)
        {
            uint8_t col = frame_buffer.buffer[i + frame_row_pointer].size;
            for (uint8_t j = 0; j < col; j++)
            {
                if (j > 79)
                    continue;
                FRAMEBUFFER_MEMORY_OFFSET[i * 160 + j * 2] = frame_buffer.buffer[frame_row_pointer + i].line_buf[j];
                FRAMEBUFFER_MEMORY_OFFSET[i * 160 + j * 2 + 1] = frame_buffer.buffer[frame_row_pointer + i].color_buf[j];
            }
        }

    }
    framebuffer_set_cursor(cursor_row, cursor_col);
}

void new_frame_buffer_view(bool change)
{
    if ((cursor_row < 0 || cursor_row >= BUFFER_HEIGHT_VIEW) || change)
    {
        if (cursor_row < 0)
        {
            frame_row_pointer--;
            cursor_row = 0;
        }
        else if (cursor_row >= BUFFER_HEIGHT_VIEW)
        {
            frame_row_pointer++;
            cursor_row = BUFFER_HEIGHT_VIEW - 1;
        }
        uint8_t row = (uint8_t)(frame_buffer.size - frame_row_pointer);
        if (row > 25)
        {
            row = 25;
        }
        framebuffer_clear();
        for (uint8_t i = 0; i < row; i++)
        {
            uint8_t col = frame_buffer.buffer[i + frame_row_pointer].size;
            for (uint8_t j = 0; j < col; j++)
            {
                if (j > 79)
                    continue;
                FRAMEBUFFER_MEMORY_OFFSET[i * 160 + j * 2] = frame_buffer.buffer[frame_row_pointer + i].line_buf[j];
                FRAMEBUFFER_MEMORY_OFFSET[i * 160 + j * 2 + 1] = frame_buffer.buffer[frame_row_pointer + i].color_buf[j];
            }
        }
    }
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg)
{
    uint8_t attr = (bg << 4) | fg;
    FRAMEBUFFER_MEMORY_OFFSET[row * 160 + col * 2] = c;
    FRAMEBUFFER_MEMORY_OFFSET[row * 160 + col * 2 + 1] = attr;
}

void put_char_color(char c, uint32_t color)
{
    handle_new_char(c, (uint8_t)(color & 0x0F), (uint8_t)((color & 0xF0) >> 4));
}

void put_char(char c, uint8_t fg, uint8_t bg)
{
    if (cursor_row + frame_row_pointer < BUFFER_MAX_HEIGHT)
    {
        if (c == '\n')
        {
            if (frame_row_pointer + cursor_row < BUFFER_MAX_HEIGHT - 1)
            {
                cursor_row++;
                frame_buffer.buffer[frame_row_pointer + cursor_row].size = 0;
                if (cursor_row + frame_row_pointer < frame_buffer.size)
                {
                    new_frame_buffer_view(true);
                }
                else
                {
                    frame_buffer.size = frame_buffer.size + 1;
                }
                new_frame_buffer_view(true);
                cursor_col = 0;
            }
        }
        else if (c != 0x0d)
        {
            if (cursor_col + 1 < BUFFER_WIDTH_VIEW || cursor_row + frame_row_pointer < BUFFER_MAX_HEIGHT - 1)
            {
                uint8_t attr = (bg << 4) | (fg);
                FRAMEBUFFER_MEMORY_OFFSET[cursor_row * 160 + cursor_col * 2] = c;
                FRAMEBUFFER_MEMORY_OFFSET[cursor_row * 160 + cursor_col * 2 + 1] = attr;
                frame_buffer.buffer[frame_row_pointer + cursor_row].line_buf[cursor_col] = c;
                frame_buffer.buffer[frame_row_pointer + cursor_row].color_buf[cursor_col] = attr;
                if (cursor_col + 1 > frame_buffer.buffer[frame_row_pointer + cursor_row].size)
                {
                    frame_buffer.buffer[frame_row_pointer + cursor_row].size = cursor_col + 1;
                }
                cursor_col++;
                if (cursor_col >= BUFFER_WIDTH_VIEW)
                {
                    frame_buffer.size = frame_buffer.size + 1;
                    cursor_row++;
                    cursor_col = 0;
                }
                new_frame_buffer_view(false);
            }
        }
        framebuffer_set_cursor(cursor_row, cursor_col);
    }
}

void puts(const char *str, uint32_t cnt, uint32_t color)
{
    for (uint32_t i = 0; i < cnt; i++)
    {
        if (str[i] == '\0')
            break;
        handle_new_char(str[i], color & 0x0F, (color & 0xF0) >> 4);
    }
}

void framebuffer_clear(void)
{
    // TODO : Implement
    size_t n = 2 * 160 * 25;
    for (size_t i = 0; i < n; i += 2)
    {
        FRAMEBUFFER_MEMORY_OFFSET[i] = 0x00;
        FRAMEBUFFER_MEMORY_OFFSET[i + 1] = 0x07;
    }
}

void init_keyboard_state(void)
{
    keyboard_state_activate();
    init_frame_buffer();
    cursor_row = 0;
    cursor_col = 0;
    frame_row_pointer = 0;
}

void framebuffer_erase(int *row, int *col)
{
    int pos = 160 * (*row) + ((*col) % 80 - 1) * 2;
    FRAMEBUFFER_MEMORY_OFFSET[pos] = 0x00;
    FRAMEBUFFER_MEMORY_OFFSET[pos + 1] = 0x07;
    if (*col > 0 || *row > 0)
    {
        (*col)--;
        if (*col < 0)
        {
            (*row)--;
            *col = 79;
        }
    }
}

void handle_new_char(char c, uint8_t fg, uint8_t bg)
{
    if (!c)
        return;
    if (cursor_row + frame_row_pointer < BUFFER_MAX_HEIGHT)
    {
        if (c == '\n')
        {
            put_char(c, fg, bg);
        }
        else if (c != '\b')
        {
            switch (c)
            {
            case ARROW_UP:
                if (cursor_row + frame_row_pointer > 0)
                {
                    cursor_row--;
                    cursor_col = cursor_col < frame_buffer.buffer[cursor_row + frame_row_pointer].size ? cursor_col : frame_buffer.buffer[cursor_row + frame_row_pointer].size;
                    new_frame_buffer_view(false);
                }
                break;
            case ARROW_DOWN:
                if (cursor_row + frame_row_pointer < frame_buffer.size - 1)
                {
                    cursor_row++;
                    cursor_col = cursor_col < frame_buffer.buffer[cursor_row + frame_row_pointer].size ? cursor_col : frame_buffer.buffer[cursor_row + frame_row_pointer].size;
                    new_frame_buffer_view(false);
                }
                break;
            case ARROW_LEFT:
                cursor_col--;
                if (cursor_col < 0)
                {
                    if (cursor_row + frame_row_pointer > 0)
                    {
                        cursor_row--;
                        cursor_col = frame_buffer.buffer[cursor_row + frame_row_pointer].size;
                        new_frame_buffer_view(false);
                    }
                    else
                    {
                        cursor_col++;
                    }
                }
                break;
            case ARROW_RIGHT:
                cursor_col++;
                if (cursor_col > frame_buffer.buffer[cursor_row + frame_row_pointer].size && cursor_row + frame_row_pointer < frame_buffer.size - 1)
                {
                    cursor_col = 0;
                    cursor_row++;
                    new_frame_buffer_view(false);
                }
                else if (cursor_col > frame_buffer.buffer[cursor_row + frame_row_pointer].size && cursor_row + frame_row_pointer >= frame_buffer.size - 1)
                {
                    cursor_col--;
                }
                break;
            default:
                put_char(c, fg, bg);
                break;
            }
        }
        else if (c == '\b')
        {
            if (cursor_col > 0 || cursor_row + frame_row_pointer > 0)
            {
                if (cursor_col == 0)
                {
                    if (frame_buffer.buffer[cursor_row + frame_row_pointer].size == 0 && cursor_row + frame_row_pointer == frame_buffer.size - 1)
                    {
                        frame_buffer.size--;
                    }
                    if (frame_buffer.buffer[cursor_row + frame_row_pointer - 1].size >= BUFFER_WIDTH_VIEW)
                    {
                        frame_buffer.buffer[cursor_row + frame_row_pointer - 1].size--;
                        framebuffer_erase(&cursor_row, &cursor_col);
                    }
                    else
                    {
                        cursor_row--;
                        new_frame_buffer_view(true);
                    }
                    cursor_col = frame_buffer.buffer[cursor_row + frame_row_pointer].size;
                }
                else
                {
                    framebuffer_erase(&cursor_row, &cursor_col);
                    frame_buffer.buffer[cursor_row + frame_row_pointer].size--;
                }
            }
        }
    }
    framebuffer_set_cursor(cursor_row, cursor_col);
}