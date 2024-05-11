#include "syscall.h"

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx)
{
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    // so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

void read_file_api(struct FAT32DriverRequest * req, uint32_t * code)
{
    syscall(0, (uint32_t)req, (uint32_t)code, 0);
}

void read_directory_api(struct FAT32DriverRequest * req, uint32_t * code)
{
    syscall(1, (uint32_t)req, (uint32_t)code, 0);
}

void write_api(struct FAT32DriverRequest * req, uint32_t * code)
{
    syscall(2, (uint32_t)req, (uint32_t)code, 0);
}

void delete_api(struct FAT32DriverRequest * req, uint32_t * code)
{
    syscall(3, (uint32_t)req, (uint32_t)code, 0);
}

void get_keyboard_char(char * c)
{
    syscall(4, (uint32_t)c, 0, 0);
}

void put_char(char c, uint32_t color)
{
    syscall(5, (uint32_t)c, color, 0);
}

void put_chars(char *c, uint32_t count, uint32_t color)
{
    syscall(6, (uint32_t)c, count, color);
}

void activate_keyboard_input()
{
    syscall(7, 0, 0, 0);
}

void clear_screen()
{
    syscall(8, 0, 0, 0);
}

void read_clusters_api(void * buf, uint32_t cluster_number, uint32_t cluster_size)
{
    syscall(9, (uint32_t)buf, cluster_number, cluster_size);
}

void set_cursor_col(uint32_t col)
{
    syscall(10, col, 0, 0);
}

void search_file_api(struct FAT32DriverRequest * req, uint32_t * code)
{
    syscall(11, (uint32_t)req, (uint32_t)code, 0);
}

void get_cluster_number_api(struct FAT32DriverRequest * req, uint32_t * code)
{
    syscall(12, (uint32_t)req, (uint32_t)code, 0);
}

void copy_folder_api(uint32_t src, uint32_t dest, uint32_t * code)
{
    syscall(13, src, dest, (uint32_t)code);
}

void copy_file_api(struct FAT32DriverRequest *src, struct FAT32DriverRequest *dest, uint32_t *code)
{
    syscall(14, (uint32_t)src, (uint32_t)dest, (uint32_t)code);void delete_recursive_api(struct FAT32DriverRequest *req, uint32_t *code);
}

void delete_recursive_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(15, (uint32_t)req, (uint32_t)code, 0);
}