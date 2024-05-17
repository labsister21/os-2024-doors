#include "command.h"

static struct ShellState state =
    {
        .work_dir = ROOT_CLUSTER_NUMBER,
        .curr_command_buffer = {'\0'},
        .curr_command_size = 0,
        .work_dir_name = "~"};

void print_working_dir()
{
    put_chars("user@doOrS", 10, 0xA);
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

    bool check;
    is_cursor_viewable_api(&check);

    if (buf == '\b' && check)
    {
        if (state.curr_command_size)
        {
            put_char(buf, 0xF);
            state.curr_command_size--;
            state.curr_command_buffer[state.curr_command_size] = '\0';
        }
    }
    else if (buf == '\n' && check)
    {
        run_command();
    }
    else if (buf == 0x11 || buf == 0x13)
    {
        bool check;
        is_shift_api(&check);
        if (check)
        {
            move_screen_api(buf);
        }
    }
    else if (buf && buf != 0x12 && buf != 0x14 && check)
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

        char cmd[256] = {0};
        memcpy(cmd, buf[0], 256);

        int cmd_len = strlen(cmd);

        // TODO: implement commands check and run command
        if (memcmp(cmd, "ls", cmd_len) == 0 && cmd_len == 2)
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
        else if (memcmp(cmd, "cd", cmd_len) == 0 && cmd_len == 2)
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
        else if ((memcmp(cmd, "clear", cmd_len) == 0 && cmd_len == 5) || (memcmp(cmd, "cls", cmd_len) == 0 && cmd_len == 3))
        {
            clear_screen();
        }
        else if (memcmp(cmd, "mkdir", cmd_len) == 0 && cmd_len == 5)
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
        else if (memcmp(cmd, "rm", cmd_len) == 0 && cmd_len == 2)
        {
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected file/folder name", 26, 0xC);
                put_char('\n', 0xC);
            }
            else if ((memcmp(buf[1], "-r", strlen(buf[1])) == 0) || memcmp(buf[1], "-R", strlen(buf[1])) == 0)
            {
                if (strlen(buf[2]) == 0)
                {
                    put_chars("Expected file/folder name", 26, 0xC);
                    put_char('\n', 0xC);
                }
                else if (strlen(buf[3]) != 0)
                {
                    put_chars("Too many arguments", 19, 0xC);
                    put_char('\n', 0xC);
                }
                else
                {
                    rm_rec(buf[2]);
                }
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
        else if (memcmp(cmd, "cp", cmd_len) == 0 && cmd_len == 2)
        {
            if (strlen(buf[1]) == 0 || strlen(buf[2]) == 0)
            {
                put_chars("Expected file/folder names", 26, 0xC);
                put_char('\n', 0xC);
            }
            else if ((memcmp(buf[1], "-r", strlen(buf[1])) == 0) || memcmp(buf[1], "-R", strlen(buf[1])) == 0)
            {
                if (strlen(buf[2]) == 0 || strlen(buf[3]) == 0)
                {
                    put_chars("Expected file/folder names", 26, 0xC);
                    put_char('\n', 0xC);
                }
                else
                {
                    cp_rec(buf[2], buf[3]);
                }
            }
            else
            {
                cp(buf[1], buf[2]);
            }
        }
        else if (memcmp(cmd, "mv", cmd_len) == 0 && cmd_len == 2)
        {
            if (strlen(buf[1]) == 0 || strlen(buf[2]) == 0)
            {
                put_chars("Expected file/folder names", 26, 0xC);
                put_char('\n', 0xC);
            }
            else
            {
                mv(buf[1], buf[2]);
            }
        }
        else if (memcmp(cmd, "cat", cmd_len) == 0 && cmd_len == 3)
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
        else if (memcmp(cmd, "help", cmd_len) == 0 && cmd_len == 4)
        {
            help();
        }
        else if (memcmp(cmd, "ps", cmd_len) == 0 && cmd_len == 2)
        {
            ps();
        }
        else if (memcmp(cmd, "kill", cmd_len) == 0 && cmd_len == 4)
        {
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected process identifier\n", 29, 0xC);
            }
            else
            {
                kill(buf[1]);
            }
        }
        else if (memcmp(cmd, "exec", cmd_len) == 0 && cmd_len == 4)
        {
            // ps();
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected binary file name\n", 27, 0xC);
            }
            else
            {
                exec(buf[1]);
            }
        }
        else if (memcmp(cmd, "./", 2) == 0)
        {
            char filename[256] = {0};
            memcpy(filename, &cmd[2], strlen(cmd));
            exec(filename);
        }
        else if (memcmp(cmd, "find", 4) == 0 && cmd_len == 4)
        {
            if (strlen(buf[1]) == 0)
            {
                put_chars("Expected target directory name", 31, 0xC);
                put_char('\n', 0xC);
            }
            else
            {
                char split_dirname[16][256] = {0};
                strsplit(buf[1], '.', split_dirname);
                char name[8] = {0};
                char ext[3] = {"\0\0\0"};
                memcpy(name, split_dirname[0], 8);
                memcpy(ext, split_dirname[1], 3);
                char curr_path[256];
                memcpy(curr_path, "~/", 2);
                bool isFound = false;
                find(ROOT_CLUSTER_NUMBER, name, ext, &isFound, curr_path);

                if (!isFound)
                {
                    put_chars("No such file or folder\n", 24, 0xC);
                }
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
    struct FAT32DirectoryTable table = {
        .table = {}};
    read_clusters_api(&table, state.work_dir, 1);

    for (int i = 2; i < 64; i++)
    {
        if (table.table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            idx++;
            print_int(idx);
            set_cursor_col(7);

            char curr_name[8] = {0};
            memcpy(&curr_name, table.table[i].name, 8);
            put_chars(curr_name, 8, 0xF);
            set_cursor_col(16);

            char curr_ext[3] = {0};
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
    char buf[16][256] = {0};
    char path[256] = {0};
    memcpy(path, state.work_dir_name, 256);
    strsplit(name, '/', buf);

    int index = 0;
    uint32_t curr_cluster = state.work_dir;
    struct FAT32DirectoryTable table = {
        .table = {},
    };

    // get path folder
    while (strlen(buf[index]) != 0)
    {
        if (strlen(buf[index]) > 8)
        {
            put_chars("The maximum length of folder name is 8 characters.\n", 51, 0xC);
            return;
        }

        read_clusters_api(&table, curr_cluster, 1);
        bool found = false;

        for (size_t i = 1; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
        {
            if (!(table.table[i].user_attribute & UATTR_NOT_EMPTY))
                continue;
            if ((memcmp(table.table[i].name, buf[index], 8) == 0) && memcmp(table.table[i].ext, "\0\0\0", 3) == 0)
            {
                if (!(table.table[i].attribute & ATTR_SUBDIRECTORY))
                {
                    put_chars("Invalid path name\n", 19, 0xC);
                    return;
                }
                else
                {
                    found = true;
                    if (memcmp(buf[index], "..", strlen(buf[index])) == 0)
                    {
                        if (curr_cluster != ROOT_CLUSTER_NUMBER)
                            move_back(path);
                    }
                    else
                    {
                        strcat(path, "/");
                        strcat(path, buf[index]);
                    }
                    curr_cluster = (table.table[i].cluster_high << 16) | table.table[i].cluster_low;
                    break;
                }
            }
        }
        if (!found)
        {
            put_chars("Invalid path name\n", 19, 0xC);
            return;
        }
        index++;
    }
    state.work_dir = curr_cluster;
    memset(state.work_dir_name, '\0', 256);
    memcpy(state.work_dir_name, path, 256);
    memset(path, '\0', 256);
}

void mkdir(char *name)
{
    if (strlen(name) > 8)
    {
        put_chars("The maximum length of folder name is 8 characters.\n", 51, 0xC);
        return;
    }
    if (contains(name, '.', strlen(name)))
    {
        put_chars("'", 1, 0xC);
        put_chars(name, strlen(name), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" contains '.' which is not allowed.\n", 36, 0xC);
        return;
    }
    struct FAT32DirectoryTable tbl = {
        .table = {}};
    struct FAT32DriverRequest request = {
        .buf = &tbl,
        .ext = "\0\0\0",
        .parent_cluster_number = state.work_dir,
        .buffer_size = 0};

    memcpy(request.name, name, 8);

    int8_t code;
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
    char name[256] = {0};
    char ext[256] = {0};
    uint32_t parent_cluster = state.work_dir;
    uint32_t curr_cluster = 0;

    int8_t code = get_curr_and_parent_cluster(filename, &parent_cluster, &curr_cluster, name, ext);
    if (code == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        return;
    }
    if (code == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (code == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (code == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    struct ClusterBuffer cb = {.buf = {0}};
    struct FAT32DriverRequest req = {
        .buf = &cb,
        .parent_cluster_number = parent_cluster,
        .buffer_size = 0};
    memcpy(req.name, name, 8);
    memcpy(req.ext, ext, 3);

    int8_t res_code;
    delete_api(&req, &res_code);

    switch (res_code)
    {
    case 0:
        put_chars("'", 1, 0xF);
        put_chars(name, strlen(name), 0xF);
        if (strlen(ext) != 0)
        {
            put_char('.', 0xF);
            put_chars(ext, 3, 0xF);
        }
        put_chars("'", 1, 0xF);
        put_chars(" has been deleted successfully.\n", 33, 0xF);
        break;
    case 1:
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        break;
    case 2:
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not an empty directory. Use recursive flag to delete this directory\n", 73, 0xC);
        break;
    default:
        put_chars("Unexpected error occured.\n", 27, 0xC);
        break;
    }
}

void rm_rec(char *foldername)
{
    char name[8] = {0};
    char ext[3] = {0};
    uint32_t parent_cluster = state.work_dir;
    uint32_t curr_cluster = 0;

    int8_t code = get_curr_and_parent_cluster(foldername, &parent_cluster, &curr_cluster, name, ext);

    if (code == 0)
    {
        put_chars("'", 1, 0xC);
        put_chars(foldername, strlen(foldername), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not a directory.\n", 29, 0xC);
        return;
    }
    if (code == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(foldername, strlen(foldername), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        return;
    }
    if (code == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (code == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (code == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    struct ClusterBuffer cb = {.buf = {0}};
    struct FAT32DriverRequest req = {
        .parent_cluster_number = parent_cluster,
        .buffer_size = 0,
        .buf = &cb,
        .ext = "\0\0\0"};
    memcpy(req.name, name, 8);
    int8_t res_code;
    delete_recursive_api(&req, &res_code);
    switch (res_code)
    {
    case 0:
        put_chars("'", 1, 0xF);
        put_chars(name, strlen(name), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" has been deleted successfully.\n", 33, 0xF);
        break;
    case 1:
        put_chars("'", 1, 0xC);
        put_chars(foldername, strlen(foldername), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not a directory.\n", 29, 0xC);
        break;
    case 2:
        put_chars("'", 1, 0xC);
        put_chars(foldername, strlen(foldername), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        break;
    default:
        put_chars("Unexpected error occured\n", 25, 0xC);
        break;
    }
}

void cat(char *filename)
{
    char name[8] = {0};
    char ext[3] = {0};
    uint32_t parent_cluster = state.work_dir;
    uint32_t curr_cluster = 0;

    int8_t check_code = get_curr_and_parent_cluster(filename, &parent_cluster, &curr_cluster, name, ext);
    if (check_code == 1)
    {
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not a file.\n", 29, 0xC);
        return;
    }
    if (check_code == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        return;
    }
    if (check_code == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (check_code == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (check_code == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    struct ClusterBuffer cb = {.buf = {0}};
    struct FAT32DriverRequest req = {
        .buf = &cb,
        .parent_cluster_number = parent_cluster,
        .buffer_size = CLUSTER_SIZE};
    memcpy(req.name, name, 8);
    memcpy(req.ext, ext, 3);

    int8_t code;
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
        break;
    }
}

void mv(char *src, char *dest)
{
    char src_name[8] = {0};
    char src_ext[3] = {0};
    uint32_t src_parent = state.work_dir;
    uint32_t src_cluster = 0;
    int8_t check_src = get_curr_and_parent_cluster(src, &src_parent, &src_cluster, src_name, src_ext);

    if (check_src == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(src, strlen(src), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        return;
    }
    if (check_src == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (check_src == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (check_src == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    char dest_name[8] = {0};
    char dest_ext[3] = {0};
    uint32_t dest_parent = state.work_dir;
    uint32_t dest_cluster = 0;
    int8_t check_dest = get_curr_and_parent_cluster(dest, &dest_parent, &dest_cluster, dest_name, dest_ext);

    if (check_dest == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (check_dest == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (check_dest == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    struct ClusterBuffer temp = {.buf = {0}};
    struct FAT32DriverRequest src_req = {
        .buf = &temp,
        .parent_cluster_number = src_parent,
        .buffer_size = 0,
    };
    memcpy(src_req.name, src_name, 8);
    memcpy(src_req.ext, src_ext, 3);

    struct FAT32DriverRequest dest_req = {
        .buf = &temp,
        .buffer_size = 0,
    };

    int8_t res;
    if (check_src == 0)
    {                        // if src is file
        if (check_dest == 1) // if dest is folder
        {
            dest_req.parent_cluster_number = dest_cluster;
            memcpy(dest_req.name, src_name, 8);
            memcpy(dest_req.ext, src_ext, 3);
        }
        else // if dest is file or not found
        {
            dest_req.parent_cluster_number = dest_parent;
            memcpy(dest_req.name, dest_name, 8);
            memcpy(dest_req.ext, dest_ext, 3);
        }
        copy_file_api(&src_req, &dest_req, &res);
        switch (res)
        {
        case 0:
            delete_api(&src_req, &res);
            put_chars("Succesfully moved file ", 24, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(src, strlen(src), 0xF);
            put_chars("'", 1, 0xF);
            put_chars(" to ", 5, 0xF);
            put_chars("'", 1, 0xF);
            put_chars(dest, strlen(dest), 0xF);
            put_chars("'\n", 2, 0xF);
            break;
        default:
            put_chars("Failed to move file\n", 21, 0xC);
            break;
        }
    }
    else // src if folder
    {
        if (check_dest == 0) // target is file
        {
            put_chars("Unable to move folder to file\n", 31, 0xC);
            return;
        }
        dest_req.parent_cluster_number = dest_parent;
        memcpy(dest_req.name, dest_name, 8);
        memcpy(dest_req.ext, dest_ext, 3);
        if (check_dest == 1) // target directory already exist
        {
            delete_recursive_api(&dest_req, &res);
            if (res != 0)
            {
                put_chars("Failed to move folder\n", 23, 0xC);
                return;
            }
        }
        write_api(&dest_req, &res);
        if (res != 0) // if write file
        {
            put_chars("Failed to move folder\n", 23, 0xC);
            return;
        }
        get_cluster_number_api(&dest_req, &dest_cluster);
        copy_folder_api(src_cluster, dest_cluster, &res);
        if (res == 0)
        {
            delete_recursive_api(&src_req, &res);
            put_chars("Succesfully moved folder ", 26, 0xF);
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
            put_chars("Failed to copy folder\n", 23, 0xC);
        }
    }
}

void cp(char *src, char *dest)
{
    char src_name[8] = {0};
    char src_ext[3] = {0};
    uint32_t src_parent = state.work_dir;
    uint32_t src_cluster = 0;
    int8_t check_src = get_curr_and_parent_cluster(src, &src_parent, &src_cluster, src_name, src_ext);

    if (check_src == 1)
    {
        put_chars("Unable to copy directory without recursive flag\n", 49, 0xC);
        return;
    }
    if (check_src == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(src, strlen(src), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        return;
    }
    if (check_src == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (check_src == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (check_src == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    char dest_name[8] = {0};
    char dest_ext[3] = {0};
    uint32_t dest_parent = state.work_dir;
    uint32_t dest_cluster = 0;
    int8_t check_dest = get_curr_and_parent_cluster(dest, &dest_parent, &dest_cluster, dest_name, dest_ext);

    if (check_dest == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (check_dest == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (check_dest == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    struct ClusterBuffer temp = {.buf = {0}};
    struct FAT32DriverRequest src_req = {
        .buf = &temp,
        .parent_cluster_number = src_parent,
        .buffer_size = 0,
    };
    memcpy(src_req.name, src_name, 8);
    memcpy(src_req.ext, src_ext, 3);

    struct FAT32DriverRequest dest_req = {
        .buf = &temp,
        .buffer_size = 0,
    };

    int8_t res;
    if (check_dest == 0 || check_dest == 2) // if target is file or not found
    {
        dest_req.parent_cluster_number = dest_parent;
        memcpy(dest_req.name, dest_name, 8);
        memcpy(dest_req.ext, dest_ext, 3);
    }
    else // if target is folder
    {
        dest_req.parent_cluster_number = dest_cluster;
        memcpy(dest_req.name, src_name, 8);
        memcpy(dest_req.ext, src_ext, 3);
    }
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

void cp_rec(char *src, char *dest)
{
    char src_name[8] = {0};
    char src_ext[3] = {0};
    uint32_t src_parent = state.work_dir;
    uint32_t src_cluster = 0;
    int8_t check_src = get_curr_and_parent_cluster(src, &src_parent, &src_cluster, src_name, src_ext);

    if (check_src == 0)
    {
        put_chars("Unable to copy file with recursive flag\n", 41, 0xC);
        return;
    }
    if (check_src == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(src, strlen(src), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        return;
    }
    if (check_src == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (check_src == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (check_src == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    char dest_name[8] = {0};
    char dest_ext[3] = {0};
    uint32_t dest_parent = state.work_dir;
    uint32_t dest_cluster = 0;
    int8_t check_dest = get_curr_and_parent_cluster(dest, &dest_parent, &dest_cluster, dest_name, dest_ext);

    if (check_dest == 0)
    {
        put_chars("Unable to copy directory to file.\n", 35, 0xC);
        return;
    }
    if (check_dest == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (check_dest == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (check_dest == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    struct ClusterBuffer temp = {.buf = {0}};
    struct FAT32DriverRequest src_req = {
        .buf = &temp,
        .parent_cluster_number = src_parent,
        .buffer_size = 0,
    };
    memcpy(src_req.name, src_name, 8);
    memcpy(src_req.ext, src_ext, 3);

    struct FAT32DriverRequest dest_req = {
        .buf = &temp,
        .buffer_size = 0,
    };

    int8_t res;
    if (check_dest == 1) // if target folder already exist
    {
        // create new subdirectory in destination dir
        struct FAT32DriverRequest dr = {
            .buf = &temp,
            .buffer_size = 0,
            .parent_cluster_number = dest_cluster,
            .ext = "\0\0\0",
        };
        memcpy(dr.name, src_name, 8);
        write_api(&dr, &res);
        if (res != 0)
        {
            put_chars("Failed to copy folder\n", 23, 0xC);
            return;
        }

        // get new directory cluster number
        uint32_t new_dest_cluster;
        get_cluster_number_api(&dr, &new_dest_cluster);
        if (!new_dest_cluster)
        {
            put_chars("Failed to copy folder\n", 23, 0xC);
            return;
        }
        copy_folder_api(src_cluster, new_dest_cluster, &res);
    }
    else // if target folder does not exist
    {
        // create folder
        memcpy(dest_req.name, dest_name, 8);
        memcpy(dest_req.ext, dest_ext, 3);
        dest_req.parent_cluster_number = dest_parent;
        write_api(&dest_req, &res);
        if (res != 0) // if write file failed
        {
            put_chars("Failed to copy folder\n", 23, 0xC);
            return;
        }
        get_cluster_number_api(&dest_req, &dest_cluster);
        copy_folder_api(src_cluster, dest_cluster, &res);
    }
    if (res == 0)
    {
        put_chars("Succesfully copied folder ", 27, 0xF);
        put_chars("'", 1, 0xF);
        put_chars(src, strlen(src), 0xF);
        put_chars("'", 1, 0xF);
        put_chars(" to ", 4, 0xF);
        put_chars("'", 1, 0xF);
        put_chars(dest, strlen(dest), 0xF);
        put_chars("'\n", 2, 0xF);
    }
    else // copy failed
    {
        put_chars("Failed to copy folder\n", 23, 0xC);
    }
}
void help()
{
    put_chars("Available commands: \n", 22, 0xB);
    put_chars("cat filename          show content of file\n", 44, 0xB);
    put_chars("cd target             move current directory to target\n", 56, 0xB);
    put_chars("clear                 clear current screen\n", 44, 0xB);
    put_chars("cls                   clear current screen\n", 44, 0xB);
    put_chars("cp [-r] source dest   copy file/folder to destination\n", 55, 0xB);
    put_chars("ls [foldername]       show current or target directory contents\n", 65, 0xB);
    put_chars("mkdir dirname         create new directory\n", 44, 0xB);
    put_chars("mv source dest        move file/folder to destination\n", 55, 0xB);
    put_chars("rm [-r] target        delete folder/ file\n", 43, 0xB);
    put_chars("find name             find file/folder with given name\n", 56, 0xB);
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

int8_t get_curr_and_parent_cluster(char *path, uint32_t *parent_cluster, uint32_t *current_cluster, char *filename, char *ext)
{
    // parse path
    char buf[16][256] = {0};
    strsplit(path, '/', buf);

    int index = 0;
    struct FAT32DirectoryTable table = {
        .table = {}};

    // get path folder until before last index
    while (strlen(buf[index + 1]) != 0)
    {
        if (strlen(buf[index]) > 8)
            return 3;

        read_clusters_api(&table, *parent_cluster, 1);

        bool found = false;

        for (size_t i = 1; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
        {
            if (!(table.table[i].user_attribute & UATTR_NOT_EMPTY))
                continue;
            if ((memcmp(table.table[i].name, buf[index], 8) == 0) && memcmp(table.table[i].ext, "\0\0\0", 3) == 0)
            {
                if (!(table.table[i].attribute & ATTR_SUBDIRECTORY))
                {
                    return 3;
                }
                else
                {
                    *parent_cluster = (table.table[i].cluster_high << 16) | table.table[i].cluster_low;
                    found = true;
                    break;
                }
            }
        }
        if (!found)
            return 3;
        index++;
    }

    // check last
    char file_buf[16][256] = {0};
    strsplit(buf[index], '.', file_buf);
    if (strlen(file_buf[0]) > 8)
        return 4;
    if (strlen(file_buf[1]) > 3)
        return 5;
    memcpy(filename, file_buf[0], 8);
    memcpy(ext, file_buf[1], 3);

    read_clusters_api(&table, *parent_cluster, 1);
    for (size_t i = 1; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(table.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if ((memcmp(table.table[i].name, file_buf[0], 8) == 0) && memcmp(table.table[i].ext, file_buf[1], 3) == 0)
        {
            *current_cluster = (table.table[i].cluster_high << 16) | table.table[i].cluster_low;
            if (!(table.table[i].attribute & ATTR_SUBDIRECTORY))
                return 0;
            else
                return 1;
        }
    }
    return 2;
}

void find(uint32_t cluster_number, char name[8], char ext[3], bool *isFound, char curr_path[256])
{
    struct FAT32DirectoryTable table = {
        .table = {}};
    read_clusters_api(&table, cluster_number, 1);

    for (size_t i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(table.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if ((memcmp(table.table[i].name, name, 8) == 0) && memcmp(table.table[i].ext, ext, 3) == 0)
        {
            *isFound = 1;
            put_chars(curr_path, strlen(curr_path), 0xF);
            put_chars(name, 8, 0xF);

            if (!(table.table[i].attribute & ATTR_SUBDIRECTORY) && memcmp(table.table[i].ext, "\0\0\0", 3) != 0)
            {
                put_char('.', 0xF);
                put_chars(ext, 3, 0xF);
            }
            put_char('\n', 0xF);
        }
        if (table.table[i].attribute & ATTR_SUBDIRECTORY)
        {
            uint32_t curr_cluster_number = (table.table[i].cluster_high << 16) | table.table[i].cluster_low;
            char new_path[256];
            memcpy(new_path, curr_path, 256);
            strcat(new_path, table.table[i].name);
            strcat(new_path, "/");
            find(curr_cluster_number, name, ext, isFound, new_path);
        }
    }
}

void exec(char *filename)
{
    char name[256] = {0};
    char ext[256] = {0};
    uint32_t parent_cluster = state.work_dir;
    uint32_t curr_cluster = 0;

    int8_t code = get_curr_and_parent_cluster(filename, &parent_cluster, &curr_cluster, name, ext);

    if (code == 1)
    {
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not a file.\n", 17, 0xC);
        return;
    }
    if (code == 2)
    {
        put_chars("'", 1, 0xC);
        put_chars(filename, strlen(filename), 0xC);
        put_chars("'", 1, 0xC);
        put_chars(" is not found.\n", 16, 0xC);
        return;
    }
    if (code == 3)
    {
        put_chars("Invalid Path.\n", 15, 0xC);
        return;
    }
    if (code == 4)
    {
        put_chars("Maximum file name length is 8.\n", 32, 0xC);
        return;
    }
    if (code == 5)
    {
        put_chars("Maximum extension name length is 3.\n", 37, 0xC);
        return;
    }

    struct FAT32DriverRequest req = {
        .buf = (uint8_t *)0,
        .buffer_size = 0x100000,
        .ext = "\0\0\0",
        .parent_cluster_number = parent_cluster,
    };
    memcpy(req.name, name, strlen(name));

    int32_t res_code;
    create_process_api(&req, &res_code);

    if (res_code == 0)
    {
        put_chars("Process created succesfully\n", 29, 0xF);
    }
    else if (res_code == 1)
    {
        put_chars("Maximum process exceeded\n", 26, 0xC);
    }
    else if (res_code == 2)
    {
        put_chars("Invalid entrypoint\n", 20, 0xC);
    }
    else if (res_code == 3)
    {
        put_chars("Not enough memory\n", 19, 0xC);
    }
    else
    {
        put_chars("File system read failure\n", 26, 0xC);
    }
}

void kill(char *pid)
{
    uint32_t id;
    if (strlen(pid) == 1)
    {
        id = *pid - '0';
    }
    else if (strlen(pid) == 2)
    {
        id = (pid[0] - '0') * 10 + (pid[1] - '0');
    }
    else
    {
        put_chars("Maximum process identifier is 16\n", 34, 0xC);
        return;
    }

    bool check;
    destroy_process_api(&id, &check);

    if (!check)
    {
        put_chars("Fail to kill process\n", 22, 0xC);
    }
    else
    {
        put_chars("Process killed succesfully\n", 28, 0xF);
    }
}

void ps()
{
    struct ProcessList pl;
    get_all_process(&pl);

    if (pl.size == 0)
    {
        put_chars("No process currently available\n", 32, 0xC);
    }
    else
    {
        put_chars("pid     name     state\n", 24, 0xF);
        put_chars("======================\n", 24, 0xF);
        for (uint32_t i = 0; i < pl.size; i++)
        {
            // print pid
            print_int(pl.metadata[i].pid);
            set_cursor_col(8);

            // print name
            put_chars(pl.metadata[i].process_name, 8, 0xF);
            set_cursor_col(17);

            if (pl.metadata[i].state == RUNNING)
            {
                put_chars("RUNNING\n", 9, 0xF);
            }
            else if (pl.metadata[i].state == READY)
            {
                put_chars("READY\n", 7, 0xF);
            }
            else
            {
                put_chars("BLOCKED\n", 9, 0xF);
            }
        }
    }
}