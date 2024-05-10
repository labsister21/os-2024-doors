#include <stdint.h>
#include "../header/filesystem/fat32.h"
#include "syscall.h"
#include "command.h"

int main(void)
{
    struct ClusterBuffer cl = {0};
    struct FAT32DriverRequest request = {
        .buf = &cl,
        .name = "shell",
        .ext = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = CLUSTER_SIZE,
    };
    uint32_t code;
    read_file_api(&request, &code);
    print_working_dir();
    activate_keyboard_input();
    while (true)
    {
        type_command();
    }
    
    return 0;
}