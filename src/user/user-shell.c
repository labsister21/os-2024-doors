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

    // char *arr = "kosong";
    // struct FAT32DriverRequest req1 = {
    //     .buf = (void *)arr,
    //     .name = "empty",
    //     .ext = "txt",
    //     .parent_cluster_number = ROOT_CLUSTER_NUMBER,
    //     .buffer_size = 6};
    // int8_t res1;
    // write_api(&req1, (uint32_t *)&res1);
    // put_char(res1 + '0', 0xF);

    // char *isi = "berto suka kucing";
    // struct FAT32DriverRequest req2 = {
    //     .buf = (void *)isi,
    //     .name = "berisi",
    //     .ext = "txt",
    //     .parent_cluster_number = ROOT_CLUSTER_NUMBER,
    //     .buffer_size = 17};
    // int8_t res2;
    // write_api(&req2, (uint32_t *)&res2);
    // put_char(res2 + '0', 0xF);

    print_working_dir();
    activate_keyboard_input();
    while (true)
    {
        type_command();
    }

    return 0;
}