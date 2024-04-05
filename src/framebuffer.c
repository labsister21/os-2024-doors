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
char c;

void init_frame_buffer() {
  frame_buffer.size = 1;
  for (uint16_t i = 0; i < BUFFER_MAX_HEIGHT; i++) {
    frame_buffer.buffer[i].size = 0;
  }
}

void framebuffer_set_cursor(uint8_t r, uint8_t c)
{
    // TODO : Implement
    uint16_t pos = 80 * r + c;
    out(CURSOR_PORT_CMD, 0x0F);
    out(CURSOR_PORT_DATA, (uint8_t)(pos & 0xFF));
    out(CURSOR_PORT_CMD, 0x0E);
    out(CURSOR_PORT_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

void new_frame_buffer_view(uint8_t fg, uint16_t bg, bool change) {
  if ((cursor_row < 0 || cursor_row >= BUFFER_HEIGHT_VIEW) || change) {
    if (cursor_row < 0) {
      frame_row_pointer--;
      cursor_row = 0;
    } else if (cursor_row >= BUFFER_HEIGHT_VIEW){
      frame_row_pointer++;
      cursor_row = BUFFER_HEIGHT_VIEW-1;
    }
    uint8_t row = (uint8_t) (frame_buffer.size-frame_row_pointer);
    if (row > 25) {
      row = 25;
    }
    framebuffer_clear();
    uint8_t attr = (bg << 4) | (fg);
    for (uint8_t i = 0; i < row; i++) {
      uint8_t col = frame_buffer.buffer[i+frame_row_pointer].size;
      for (uint8_t j = 0; j < col ; j++) {
        if (j > 79) continue;
        FRAMEBUFFER_MEMORY_OFFSET[i*160 + j*2] = frame_buffer.buffer[frame_row_pointer+i].line_buf[j];
        FRAMEBUFFER_MEMORY_OFFSET[i*160 + j*2 + 1] = attr;
      }
    }
  }
}

void framebuffer_write(char c, uint8_t fg, uint8_t bg)
{
    // TODO : Implement
    if (cursor_row + frame_row_pointer < BUFFER_MAX_HEIGHT){
      if (c == '\n') {
        if (frame_row_pointer+cursor_row < BUFFER_MAX_HEIGHT-1) {
          cursor_row ++;
          frame_buffer.buffer[frame_row_pointer+cursor_row].size = 0;
          if (cursor_row < frame_buffer.size) {
            new_frame_buffer_view(fg, bg, true);
          } else {
            frame_buffer.size = frame_buffer.size + 1;
          }
          new_frame_buffer_view(fg, bg, true);
          cursor_col = 0;
        }
      } else if (c != 0x0d) {
        if (cursor_col+1 < 80 || cursor_row + frame_row_pointer < BUFFER_MAX_HEIGHT - 1) {
            uint8_t attr = (bg << 4) | (fg);
            FRAMEBUFFER_MEMORY_OFFSET[cursor_row * 160 + cursor_col * 2] = c;
            FRAMEBUFFER_MEMORY_OFFSET[cursor_row * 160 + cursor_col * 2 + 1] = attr;
            frame_buffer.buffer[frame_row_pointer+cursor_row].line_buf[cursor_col] = c;
            if (cursor_col+1 > frame_buffer.buffer[frame_row_pointer+cursor_row].size) {
              frame_buffer.buffer[frame_row_pointer+cursor_row].size = cursor_col+1;
            }
            cursor_col++;
            if (cursor_col >= BUFFER_WIDTH_VIEW) {
              frame_buffer.size = frame_buffer.size + 1;
              cursor_row++;
              cursor_col = 0;
            }
            new_frame_buffer_view(fg, bg, false);
        }
      } 
      framebuffer_set_cursor(cursor_row, cursor_col);
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

void init_keyboard_state(void) {
    keyboard_state_activate();
    init_frame_buffer();
    cursor_row = 0;
    cursor_col = 0;
    frame_row_pointer = 0;
    c = 0;
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

void print(int row, int col) {
  int past_row = cursor_row;
  int past_col = cursor_col;
  if (row == 0) {
    framebuffer_write('0', 0xF, 0);
  } else {
    char str[20];
    uint8_t length = 0;
    while (row > 0) {
      str[length] = row%10 + '0';
      length++;
      row = row/10;
    }
    for (int i = length-1; i >= 0; i--) {
      framebuffer_write(str[i], 0xF, 0);
    }
  }
  framebuffer_write(',', 0xF, 0);
  if (col == 0) {
    framebuffer_write('0', 0xF, 0);
  } else {
    char str[20];
    uint8_t length = 0;
    while (col > 0) {
      str[length] = col%10 + '0';
      length++;
      col = col/10;
    }
    for (int i = length-1; i >= 0; i--) {
      framebuffer_write(str[i], 0xF, 0);
    }
  }
  cursor_row = past_row;
  cursor_col = past_col;
  framebuffer_set_cursor(cursor_row, cursor_col);
}


void typing_keyboard() {
    if (c){
      if (c != '\b') {
        if (c != '\n') {
          framebuffer_write(*row, *col, c, 0xF, 0);
          (*col)++;
          (*row) += (*col)/80;
          (*col) %= 80;
          framebuffer_set_cursor(*row, *col);
        } else {
          if (*col != 79) {
            new_line_table.table[new_line_table.size].row = (uint8_t) (*row);
            new_line_table.table[new_line_table.size].col = (uint8_t) (*col);
            new_line_table.size++;
          }
          (*col) = 0;
          (*row) ++;
          framebuffer_set_cursor(*row, *col);
        }
      } else if (*col > 0 || *row > 0) {
        if (*col == 0 && new_line_table.size > 0) {
          (*row)--;
          (*col) = new_line_table.table[new_line_table.size-1].col;
          new_line_table.size--;
          framebuffer_set_cursor(*row, *col);
        } else {
          framebuffer_erase(row, col);
          framebuffer_set_cursor(*row, *col);
        }
      }
    }
}