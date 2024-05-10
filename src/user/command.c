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

        char buf[16][256] = {0};
        strsplit(state.curr_command_buffer, ' ', buf);

        char cmd[256];
        memcpy(cmd, buf[0], 256);

        int cmd_len = strlen(cmd);

        // TODO: implement commands check and run command
        if (memcmp(cmd, "ls", cmd_len) == 0)
        {
            if (strlen(buf[1]) == 0)
            {
                ls();
            }
            else
            {
                put_chars("'", 1, 0xF);
                put_chars(cmd, cmd_len, 0xF);
                put_chars("'", 1, 0xF);
                put_chars(" only receive one argument.", 24, 0xF);
                put_char('\n', 0xF);
            }
        }
        else if (memcmp(cmd, "cd", cmd_len) == 0)
        {
            // cd()
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected directory name", 23, 0xF);
                put_char('\n', 0xF);
            }
            else
            {
                cd(buf[1]);
            }
        }
        else if (memcmp(cmd, "clear", cmd_len) == 0 || memcmp(cmd, "cls", cmd_len) == 0)
        {
            clear_screen();
        }
        else if (memcmp(cmd, "mkdir", cmd_len) == 0)
        {
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected directory name", 23, 0xF);
                put_char('\n', 0xF);
            }
            else if (strlen(buf[2]) != 0)
            {
                put_chars("Too many arguments", 18, 0xF);
                put_char('\n', 0xF);
            }
            else
            {
                mkdir(buf[1]);
            }
        }
        else if (memcmp(cmd, "cp", cmd_len) == 0)
        {
            if (strlen(buf[1]) == 0 || strlen(buf[2]) == 0)
            {
                put_chars("Expected file/folder names", 26, 0xF);
                put_char('\n', 0xF);
            }
            else
            {
                cp(buf[1], buf[2]);
            }
        }
        else
        {
            put_chars("'", 1, 0xF);
            put_chars(cmd, cmd_len, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(" is not recognized as an internal command.", 42, 0xF);
            put_char('\n', 0xF);
        }
    }
    else
    {
        put_char('\n', 0xF);
    }
    print_working_dir();
}

void ls()
{
    int idx = 0;
    put_chars("no   name     type\n", 28, 0xF);
    put_chars("------------------\n", 28, 0xF);

    // get directory table
    struct FAT32DirectoryTable table;
    read_clusters_api(&table, state.work_dir, 1);

    for (int i = 2; i < 64; i++)
    {
        if (table.table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            idx++;
            put_char('0' + idx, 0xF);
            set_cursor_col(5);

            char curr_name[8];
            memcpy(&curr_name, table.table[i].name, 8);
            put_chars(curr_name, 8, 0xF);
            set_cursor_col(14);

            if (table.table[i].attribute == ATTR_SUBDIRECTORY)
            {
                put_chars("folder", 6, 0xF);
            }
            else
            {
                put_chars("file", 4, 0xF);
            }

            put_char('\n', 0xF);
        }
    }
}

void cd(char *name)
{
    struct FAT32DirectoryTable table;
    struct FAT32DriverRequest request = {
        .buf = &table,
        .ext = "\0\0\0",
        .parent_cluster_number = state.work_dir,
        .buffer_size = CLUSTER_SIZE};

    memcpy(request.name, name, 8);

    uint32_t code;
    read_directory_api(&request, &code);

    if (code == 0)
    {
        if (memcmp(name, "..", strlen(name)) == 0)
        {
            if (state.work_dir != ROOT_CLUSTER_NUMBER)
            {
                move_back(state.work_dir_name);
            }
        }
        else
        {
            strcat(state.work_dir_name, "/");
            strcat(state.work_dir_name, name);
        }
        state.work_dir = (table.table[0].cluster_high << 16) | (table.table[0].cluster_low);
    }
    else if (code == 1)
    {
        put_chars("'", 1, 0xF);
        put_chars(name, strlen(name), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" is not a folder.\n", 18, 0xF);
    }
    else if (code == 2)
    {
        put_chars("'", 1, 0xF);
        put_chars(name, strlen(name), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" is not found.\n", 15, 0xF);
    }
    else
    {
        put_chars("Unexpected error occured.\n", 26, 0xF);
    }
}

void mkdir(char *name)
{
    if (strlen(name) > 8)
    {
        put_chars("The maximum length of folder name is 8 characters.\n", 51, 0xF);
    }
    if (contains(name, '.', strlen(name)))
    {
        put_chars("'", 1, 0xF);
        put_chars(name, strlen(name), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" contains '.' which is not allowed.\n", 36, 0xF);
        return;
    }
    struct FAT32DirectoryTable table;
    struct FAT32DriverRequest request = {
        .buf = &table,
        .ext = "\0\0\0",
        .parent_cluster_number = state.work_dir,
        .buffer_size = 0};

    memcpy(request.name, name, 8);

    uint32_t code;
    write_api(&request, &code);

    if (code == 0)
    {
        put_chars("'", 1, 0xF);
        put_chars(name, strlen(name), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" is created successfully.\n", 26, 0xF);
    }
    else if (code == 1)
    {
        put_chars("'", 1, 0xF);
        put_chars(name, strlen(name), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" already exists.\n", 17, 0xF);
    }
    else if (code == 2)
    {
        put_chars("Parent folder is undefined\n", 27, 0xF);
    }
    else
    {
        put_chars("Unexpected error occured\n", 25, 0xF);
    }
}

void move_back(char *c)
{
    int i = 255;
    while (i >= 0)
    {
        if (c[i] == '/')
        {
            c[i] = '\0';
            break;
        }
        c[i] = '\0';
        i--;
    }
}

void cp(char *src, char *dest)
{
    char src_buf[16][256] = {0};
    strsplit(src, '.', src_buf);
    char dest_buf[16][256] = {0};
    strsplit(dest, '.', dest_buf);

    // check source
    struct FAT32DriverRequest src_req = {
        .parent_cluster_number = state.work_dir,
    };
    memcpy(src_req.name, src_buf[0], 8);
    memcpy(src_req.ext, src_buf[1], 3);

    uint32_t src_code;
    search_file_api(&src_req, &src_code);

    // check dest
    struct FAT32DriverRequest dest_req = {
        .parent_cluster_number = state.work_dir,
    };
    memcpy(dest_req.name, dest_buf[0], 8);
    memcpy(dest_req.ext, dest_buf[1], 3);
    uint32_t dest_code;
    search_file_api(&dest_req, &dest_code);

    uint32_t res;
    if (src_code == 0 && src_code == dest_code) // handle file copy
    {
        copy_file_api(&src_req, &dest_req, &res);
        if (res == 0)
        {
            put_chars("Succesfully copied file ", 23, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(src, strlen(src), 0xF);
            put_chars("'", 1, 0xF);
            put_chars(" to ", 4, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(dest, strlen(dest), 0xF);
            put_chars("'\n", 2, 0xF);
        }
        else
        {
            put_chars("Failed to copy file\n", 21, 0xF);
        }
    }
    else if (src_code == 1 && src_code == dest_code) // handle folder copy
    {
        uint32_t src_cluster;
        uint32_t dest_cluster;
        get_cluster_number_api(&src_req, &src_cluster);
        get_cluster_number_api(&dest_req, &dest_cluster);

        if (!src_cluster || !dest_cluster)
        {
            put_chars("Failed to copy folder\n", 22, 0xF);
            return;
        }

        copy_folder_api(src_cluster, dest_cluster, &res);
        if (res == 0)
        {
            put_chars("Succesfully copied folder ", 26, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(src, strlen(src), 0xF);
            put_chars("'", 1, 0xF);
            put_chars(" to ", 4, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(dest, strlen(dest), 0xF);
            put_chars("'\n", 2, 0xF);
        }
        else
        {
            put_chars("Failed to copy folder\n", 22, 0xF);
        }
    }
    else if ((src_code == 0 && dest_code == 1) || (src_code == 1 && dest_code == 0)) // handle file
    {
        put_chars("'", 1, 0xF);
        put_chars(src, strlen(src), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" and ", 5, 0xF);
        put_chars("'", 1, 0xF);
        put_chars(dest, strlen(dest), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" are of different types.\n", 25, 0xF);
    }
    else if (src_code == 2)
    {
        put_chars("'", 1, 0xF);
        put_chars(src, strlen(src), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" is not found.\n", 15, 0xF);
    }
    else if (dest_code == 2)
    {
        put_chars("'", 1, 0xF);
        put_chars(dest, strlen(dest), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" is not found.\n", 15, 0xF);
    }
    else
    {
        put_chars("Unexpected error occured\n", 22, 0xF);
    }
}