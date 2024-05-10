#include "command.h"

static struct ShellState state =
    {
        .work_dir = ROOT_CLUSTER_NUMBER,
        .curr_command_buffer = {'\0'},
        .curr_command_size = 0,
        .work_dir_name = "~"};

void print_working_dir()
{
    put_chars("user@doOrS", 10, 0x2);
    put_chars(":", 1, 0x7);
    put_chars(state.work_dir_name, 256, 0x9);
    put_chars("$ ", 2, 0x7);
    state.curr_command_size = 0;
    memset((void *)(&state.curr_command_buffer), '\0', 256);
}

void type_command()
{
    char buf;
    get_keyboard_char(&buf);

    if (buf == '\b')
    {
        if (state.curr_command_size)
        {
            put_char(buf, 0xF);
            state.curr_command_size--;
            state.curr_command_buffer[state.curr_command_size] = '\0';
        }
    }
    else if (buf == '\n')
    {
        run_command();
    }
    else if (buf)
    {
        if (state.curr_command_size < 256)
        {
            put_char(buf, 0xF);
            state.curr_command_buffer[state.curr_command_size] = buf;
            state.curr_command_size++;
        }
    }
}

void run_command()
{
    if (state.curr_command_size)
    {
        put_char('\n', 0xF);

        // TODO: implement commands check and run command
        if (memcmp(state.curr_command_buffer, "ls", state.curr_command_size) == 0)
        {
            put_chars("berhasil ls", 11, 0xF);
        }
        else if (memcmp(state.curr_command_buffer, "cd", state.curr_command_size) == 0)
        {
            put_chars("berhasil cd", 11, 0xF);
        }
        else 
        {
            put_chars("'", 1, 0xF);
            put_chars(state.curr_command_buffer, state.curr_command_size, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(" is not recognized as an internal command.", 42, 0xF);
        }
    }

    put_char('\n', 0xF);
    print_working_dir();
}