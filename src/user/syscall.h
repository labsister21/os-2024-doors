#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stddef.h>
#include "../header/filesystem/fat32.h"
#include "../header/stdlib/string.h"

typedef enum PROCESS_STATE
{
    BLOCKED,
    READY,
    RUNNING
} PROCESS_STATE;

struct ProcessList
{
    struct
    {
        uint32_t pid;
        PROCESS_STATE state;
        char process_name[8];
    } metadata[16];
    uint32_t size;
};

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

void put_char(char c, uint32_t color);

void put_chars(char *c, uint32_t count, uint32_t color);

void clear_screen();

void activate_keyboard_input();

void read_file_api(struct FAT32DriverRequest *req, int8_t *code);

void read_directory_api(struct FAT32DriverRequest *req, int8_t *code);

void read_clusters_api(void *buf, uint32_t cluster_number, uint32_t cluster_size);

void write_api(struct FAT32DriverRequest *req, int8_t *code);

void delete_api(struct FAT32DriverRequest *req, int8_t *code);

void get_keyboard_char(char *c);

void set_cursor_col(uint32_t col);

void search_file_api(struct FAT32DriverRequest *req, int8_t *code);

void get_cluster_number_api(struct FAT32DriverRequest *req, uint32_t *code);

void copy_folder_api(uint32_t src, uint32_t dest, int8_t *code);

void copy_file_api(struct FAT32DriverRequest *src, struct FAT32DriverRequest *dest, int8_t *code);

void delete_recursive_api(struct FAT32DriverRequest *req, int8_t *code);

void move_screen_api(char c);

void is_shift_api(bool *check);

void is_cursor_viewable_api(bool *check);

void destroy_process_api(uint32_t *pid, bool *check);

void create_process_api(struct FAT32DriverRequest *req, int32_t *code);

void get_all_process(struct ProcessList *pl);

void read_rtc(uint16_t *year, uint16_t *month, uint16_t *day, uint16_t *hour, uint16_t *minute, uint16_t *second);

void put_char_position(uint32_t row, uint32_t col, char c);

void get_time_stamp(uint32_t *time);

#endif