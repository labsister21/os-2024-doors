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

void read_file_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(0, (uint32_t)req, (uint32_t)code, 0);
}

void read_directory_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(1, (uint32_t)req, (uint32_t)code, 0);
}

void write_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(2, (uint32_t)req, (uint32_t)code, 0);
}

void delete_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(3, (uint32_t)req, (uint32_t)code, 0);
}

void get_keyboard_char(char *c)
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

void read_clusters_api(void *buf, uint32_t cluster_number, uint32_t cluster_size)
{
    syscall(9, (uint32_t)buf, cluster_number, cluster_size);
}

void set_cursor_col(uint32_t col)
{
    syscall(10, col, 0, 0);
}

void search_file_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(11, (uint32_t)req, (uint32_t)code, 0);
}

void get_cluster_number_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(12, (uint32_t)req, (uint32_t)code, 0);
}

void copy_folder_api(uint32_t src, uint32_t dest, uint32_t *code)
{
    syscall(13, src, dest, (uint32_t)code);
}

void copy_file_api(struct FAT32DriverRequest *src, struct FAT32DriverRequest *dest, uint32_t *code)
{
    syscall(14, (uint32_t)src, (uint32_t)dest, (uint32_t)code);
}

void delete_recursive_api(struct FAT32DriverRequest *req, uint32_t *code)
{
    syscall(15, (uint32_t)req, (uint32_t)code, 0);
}

void move_screen_api(char c)
{
    syscall(16, (uint32_t)c, 0, 0);
}

void is_shift_api(bool *check)
{
    syscall(17, (uint32_t)check, 0, 0);
}

void is_cursor_viewable_api(bool *check)
{
    syscall(18, (uint32_t)check, 0, 0);
}

void destroy_process_api(uint32_t *pid, bool *check)
{
    syscall(20, (uint32_t)pid, (uint32_t)check, 0);
}

void create_process_api(struct FAT32DriverRequest *req, int32_t *code)
{
    syscall(21, (uint32_t)req, (uint32_t)code, 0);
}

void get_all_process(struct ProcessList *pl)
{
    syscall(22, (uint32_t)pl, 0, 0);
}

void read_rtc(uint16_t *year, uint16_t *month, uint16_t *day, uint16_t *hour, uint16_t *minute, uint16_t *second)
{
    uint16_t *array[] = {year, month, day, hour, minute, second};
    syscall(23, (uint32_t)array, 0, 0);
}

void put_char_position(uint32_t row, uint32_t col, char c)
{
    syscall(24, (uint32_t)row, (uint32_t)col, (uint32_t)c);
}

void get_time_stamp(uint32_t * time)
{
    syscall(25, (uint32_t)time, 0, 0);
}