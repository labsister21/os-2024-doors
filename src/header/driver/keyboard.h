#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/cpu/interrupt.h"

#define EXT_SCANCODE_UP 0x48 // 72
#define EXT_SCANCODE_DOWN 0x50 // 80
#define EXT_SCANCODE_LEFT 0x4B // 75
#define EXT_SCANCODE_RIGHT 0x4D // 77

#define ARROW_UP 0x11
#define ARROW_LEFT 0x12
#define ARROW_DOWN 0x13
#define ARROW_RIGHT 0x14

#define KEYBOARD_DATA_PORT 0x60
#define EXTENDED_SCANCODE_BYTE 0xE0

/**
 * keyboard_scancode_1_to_ascii_map[256], Convert scancode values that correspond to ASCII printables
 * How to use this array: ascii_char = k[scancode]
 *
 * By default, QEMU using scancode set 1 (from empirical testing)
 */
extern const char keyboard_scancode_1_to_ascii_map[256];

/**
 * KeyboardDriverState - Contain all driver states
 *
 * @param read_extended_mode Optional, can be used for signaling next read is extended scancode (ex. arrow keys)
 * @param keyboard_input_on  Indicate whether keyboard ISR is activated or not
 * @param keyboard_buffer    Storing keyboard input values in ASCII
 */
struct KeyboardDriverState
{
    bool read_extended_mode;
    bool keyboard_input_on;
    bool capslock;
    bool left_shift;
    bool right_shift;
    char keyboard_buffer;
} __attribute((packed));

/* -- Driver Interfaces -- */

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void);

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void);

// Get keyboard buffer value and flush the buffer - @param buf Pointer to char buffer
void get_keyboard_buffer(char *buf);

bool is_shift();

/* -- Keyboard Interrupt Service Routine -- */

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 */
void keyboard_isr(void);

bool is_keyboard_active();

#endif