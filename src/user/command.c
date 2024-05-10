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
                put_chars("'", 1, 0xC);
                put_chars(cmd, cmd_len, 0xC);
                put_chars("'", 1, 0xC);
                put_chars(" only receive one argument.", 24, 0xC);
                put_char('\n', 0xC);
            }
        }
        else if (memcmp(cmd, "cd", cmd_len) == 0)
        {
            // cd()
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected directory name", 23, 0xC);
                put_char('\n', 0xC);
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
                put_chars("Expected directory name", 23, 0xC);
                put_char('\n', 0xC);
            }
            else if (strlen(buf[2]) != 0)
            {
                put_chars("Too many arguments", 18, 0xC);
                put_char('\n', 0xC);
            }
            else
            {
                mkdir(buf[1]);
            }
        }
        else if (memcmp(cmd, "rm", cmd_len) == 0)
        {
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected file/folder name", 26, 0xC);
                put_char('\n', 0xC);
            }
            else if (strlen(buf[2]) != 0)
            {
                put_chars("Too many arguments", 19, 0xC);
                put_char('\n', 0xC);
            }
            else
            {
                rm(buf[1]);
            }
        }
        else if (memcmp(cmd, "cp", cmd_len) == 0)
        {
            if (strlen(buf[1]) == 0 || strlen(buf[2]) == 0)
            {
                put_chars("Expected file/folder names", 26, 0xC);
                put_char('\n', 0xC);
            }
            else
            {
                cp(buf[1], buf[2]);
            }
        }
        else if (memcmp(cmd, "cat", cmd_len) == 0)
        {
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected file name", 18, 0xC);
                put_char('\n', 0xC);
            }
            else
            {
                cat(buf[1]);
            }
        }
        else
        {
            put_chars("'", 1, 0xC);
            put_chars(cmd, cmd_len, 0xC);
            put_chars("'", 1, 0xC);
            put_chars(" is not recognized as an internal command.", 42, 0xC);
            put_char('\n', 0xC);
        }
    }
    else
    {
        put_char('\n', 0xC);
    }
    print_working_dir();
}

void ls()
{
    int idx = 0;
    put_chars("no     name     ext     type     size\n", 39, 0xF);
    put_chars("=====================================\n", 39, 0xF);

    // get directory table
    struct FAT32DirectoryTable table;
    read_clusters_api(&table, state.work_dir, 1);

    for (int i = 2; i < 64; i++)
    {
        if (table.table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            idx++;
            put_char('0' + idx, 0xF);
            set_cursor_col(7);

            char curr_name[8];
            memcpy(&curr_name, table.table[i].name, 8);
            put_chars(curr_name, 8, 0xF);
            set_cursor_col(16);

            char curr_ext[3];
            memcpy(&curr_ext, table.table[i].ext, 3);
            put_chars(curr_ext, 3, 0xF);
            set_cursor_col(24);

            if (table.table[i].attribute == ATTR_SUBDIRECTORY)
            {
                put_chars("folder", 6, 0xF);
            }
            else
            {
                put_chars("file", 4, 0xF);
            }
            set_cursor_col(33);

            print_int(table.table[i].filesize);

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
        put_chars("'", 1, 0xC);
        put_chars(name, strlen(name), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not a folder.\n", 18, 0xC);
    }
    else if (code == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(name, strlen(name), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 15, 0xC);
    }
    else
    {
        put_chars("Unexpected error occured.\n", 26, 0xC);
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
        put_chars("'", 1, 0xC);
        put_chars(name, strlen(name), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" contains '.' which is not allowed.\n", 36, 0xC);
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
        put_chars("'", 1, 0xC);
        put_chars(name, strlen(name), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" already exists.\n", 17, 0xC);
    }
    else if (code == 2)
    {
        put_chars("Parent folder is undefined\n", 27, 0xC);
    }
    else
    {
        put_chars("Unexpected error occured\n", 25, 0xC);
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

void rm(char *filename)
{
    char filename_buf[16][256] = {0};
    strsplit(filename, '.', filename_buf);

    struct FAT32DriverRequest req = {
        .parent_cluster_number = state.work_dir,
    };
    memcpy(req.name, filename_buf[0], 8);
    memcpy(req.ext, filename_buf[1], 3);

    uint32_t code;
    delete_api(&req, &code);

    switch (code)
    {
    case 0:
        put_chars("'", 1, 0xF);
        put_chars(filename, strlen(filename), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" has been deleted successfully.\n", 33, 0xF);
        break;
    case 1:
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 15, 0xC);
        break;
    default:
        put_chars("Unexpected error occured\n", 25, 0xC);
        break;
    }
}

void cat(char *filename)
{
    struct FAT32DirectoryTable table;
    struct FAT32DriverRequest req = {
        .buf = &table,
        .parent_cluster_number = state.work_dir,
        .buffer_size = CLUSTER_SIZE};

    char split_filename[16][256] = {0};
    strsplit(filename, '.', split_filename);

    memcpy(req.name, split_filename[0], 8);
    memcpy(req.ext, split_filename[1], 3);

    uint32_t code;
    read_file_api(&req, &code);

    switch (code)
    {
    case 0:
        put_chars(req.buf, req.buffer_size, 0xB);
        put_char('\n', 0x7);
        break;
    case 1:
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not a file.\n", 17, 0xC);
        break;
    case 2:
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        break;
    default:
        put_chars("unexpected error occured\n", 26, 0xC);
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
            put_chars("Failed to copy file\n", 21, 0xC);
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
            put_chars("Failed to copy folder\n", 22, 0xC);
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
        put_chars("'", 1, 0xC);
        put_chars(src, strlen(src), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" and ", 5, 0xC);
        put_chars("'", 1, 0xC);
        put_chars(dest, strlen(dest), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" are of different types.\n", 25, 0xC);
    }
    else if (src_code == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(src, strlen(src), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 15, 0xC);
    }
    else if (dest_code == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(dest, strlen(dest), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 15, 0xC);
    }
    else
    {
        put_chars("Unexpected error occured\n", 22, 0xC);
    }
}

void print_int(uint32_t num)
{
    uint32_t temp = 0;
    if (num == 0)
    {
        put_char('0', 0xF);
    }
    while (num > 0)
    {
        temp = (temp * 10) + (num % 10);
        num /= 10;
    }
    while (temp > 0)
    {
        put_char((temp % 10) + '0', 0xF);
        temp /= 10;
    }
}