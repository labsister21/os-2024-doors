#include "header/driver/keyboard.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/interrupt.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"

static struct KeyboardDriverState keyboard_state;

const char keyboard_scancode_1_to_ascii_map[256] = {
    0,
    0x1B,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '\b',
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n',
    0,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    0,
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0,
    '*',
    0,
    ' ',
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    '-',
    0,
    0,
    0,
    '+',
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

char shift_map[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // b
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // c
    0, 0, 0, 0, 0, 0, 0, 0, 0, '"',
    0, 0, 0, 0, '<', '_', '>', '?', ')', '!',
    '@', '#', '$', '%', '^', '&', '*', '(', 0, ':',
    0, '+', 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, '{', '|', '}', 0, 0, '~'

};

/* -- Driver Interfaces -- */

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void)
{
    keyboard_state.keyboard_input_on = true;
    keyboard_state.capslock = false;
    keyboard_state.left_shift = false;
    keyboard_state.right_shift = false;
}

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void)
{
    keyboard_state.keyboard_input_on = false;
}

// Get keyboard buffer value and flush the buffer - @param buf Pointer to char buffer
void get_keyboard_buffer(char *buf)
{
    *buf = keyboard_state.keyboard_buffer;
    keyboard_state.keyboard_buffer = '\0';
}

bool is_shift()
{
    return keyboard_state.left_shift | keyboard_state.right_shift;
}

/* -- Keyboard Interrupt Service Routine -- */

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 */
void keyboard_isr(void)
{
    uint8_t scancode = in(KEYBOARD_DATA_PORT);
    // TODO : Implement scancode processing
    if (keyboard_state.keyboard_input_on)
    {

        if (!(scancode & 0x80))
        {
            if (scancode == 0x3A)
            {
                keyboard_state.capslock = !keyboard_state.capslock;
            }
            else if (scancode == 0x2A)
            {
                keyboard_state.left_shift = true;
            }
            else if (scancode == 0x36)
            {
                keyboard_state.right_shift = true;
            }
            else if (scancode == EXT_SCANCODE_UP)
            {
                keyboard_state.keyboard_buffer = ARROW_UP;
            }
            else if (scancode == EXT_SCANCODE_LEFT)
            {
                keyboard_state.keyboard_buffer = ARROW_LEFT;
            }
            else if (scancode == EXT_SCANCODE_DOWN)
            {
                keyboard_state.keyboard_buffer = ARROW_DOWN;
            }
            else if (scancode == EXT_SCANCODE_RIGHT)
            {
                keyboard_state.keyboard_buffer = ARROW_RIGHT;
            }
            else
            {
                char mapped_ascii = keyboard_scancode_1_to_ascii_map[scancode];
                if ((keyboard_state.capslock ^ is_shift()) && 97 <= mapped_ascii && mapped_ascii <= 122)
                {
                    mapped_ascii -= 32;
                }
                else if (is_shift() && mapped_ascii < 97 && shift_map[(uint8_t)mapped_ascii] != 0)
                {
                    mapped_ascii = shift_map[(uint8_t)mapped_ascii];
                }
                keyboard_state.keyboard_buffer = mapped_ascii;
            }
        }
        else
        {
            if (scancode == 0xAA)
            {
                keyboard_state.left_shift = false;
            }
            else if (scancode == 0xB6)
            {
                keyboard_state.right_shift = false;
            }
        }
    }
    pic_ack(IRQ_KEYBOARD);
    return;
}

bool is_keyboard_active()
{
    return keyboard_state.keyboard_input_on;
}