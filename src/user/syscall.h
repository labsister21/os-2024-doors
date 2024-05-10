#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stddef.h>
#include "../header/filesystem/fat32.h"
#include "../header/stdlib/string.h"

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

void put_char(char c, uint32_t color);

void put_chars(char * c, uint32_t count, uint32_t color);

void clear_screen();

void activate_keyboard_input();

void read_file_api(struct FAT32DriverRequest * req, uint32_t * code);

void read_directory_api(struct FAT32DriverRequest * req, uint32_t * code);

void read_clusters_api(void * buf, uint32_t cluster_number, uint32_t cluster_size);

void write_api(struct FAT32DriverRequest * req, uint32_t * code);

void delete_api(struct FAT32DriverRequest * req, uint32_t * code);

void get_keyboard_char(char * c);

void set_cursor_col(uint32_t col);

#endif