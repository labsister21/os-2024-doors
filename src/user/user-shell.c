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
    int8_t code;
    read_file_api(&request, &code);

    char *arr = "kosong";
    struct FAT32DriverRequest req1 = {
        .buf = (void *)arr,
        .name = "empty",
        .ext = "txt",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = 6};
    int8_t res1;
    write_api(&req1, &res1);

    char *isi = "Nandemonai to kuchi wo tsugunda\nHonto wa chotto ashi wo tometakute\nDakedomo kimi wa haya ashi\nDe sutto mae wo iku kara\nBoku wa sore wo mitsumeteru\n\nSaishuubin kimi wa noru boku wo oitette\nHashiridasu yukkuri to\nJimen ga zurete ikut\nNaicha dame naicha dame\nDemo honto wa iitai yo\nikanaide";
    struct FAT32DriverRequest req2 = {
        .buf = (void *)isi,
        .name = "ikanaide",
        .ext = "txt",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = 288};
    int8_t res2;
    write_api(&req2, &res2);

    print_working_dir();
    activate_keyboard_input();
    while (true)
    {
        type_command();
    }
    return 0;
}