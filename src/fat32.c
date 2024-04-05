#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"
#include "header/filesystem/fat32.h"

static struct FAT32DriverState fat32_driver_state;

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C',
    'o',
    'u',
    'r',
    's',
    'e',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'D',
    'e',
    's',
    'i',
    'g',
    'n',
    'e',
    'd',
    ' ',
    'b',
    'y',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'L',
    'a',
    'b',
    ' ',
    'S',
    'i',
    's',
    't',
    'e',
    'r',
    ' ',
    'I',
    'T',
    'B',
    ' ',
    ' ',
    'M',
    'a',
    'd',
    'e',
    ' ',
    'w',
    'i',
    't',
    'h',
    ' ',
    '<',
    '3',
    ' ',
    ' ',
    ' ',
    ' ',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '2',
    '0',
    '2',
    '4',
    '\n',
    [BLOCK_SIZE - 2] = 'O',
    [BLOCK_SIZE - 1] = 'k',
};

uint32_t cluster_to_lba(uint32_t cluster)
{
    return cluster * CLUSTER_BLOCK_COUNT;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    char * c = "..";
    memcpy(dir_table->table[0].name, name, sizeof(name));
    memcpy(dir_table->table[1].name, c, sizeof(c));
    dir_table->table[1].cluster_low = (uint16_t)parent_dir_cluster;
    dir_table->table[1].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[1].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    // for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    // {
    //     memset()
    // }
}

bool is_empty_storage(void)
{
    struct BlockBuffer b;
    read_blocks(&b, BOOT_SECTOR, 1);
    return memcmp((const void *)&b, fs_signature, sizeof(fs_signature)) != 0;
}

void create_fat32(void)
{
    write_blocks(fs_signature, BOOT_SECTOR, 1);
    struct FAT32FileAllocationTable fat;
    fat.cluster_map[0] = CLUSTER_0_VALUE;
    fat.cluster_map[1] = CLUSTER_1_VALUE;
    fat.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    for (int i = 3; i < CLUSTER_MAP_SIZE; i++)
    {
        fat.cluster_map[i] = FAT32_FAT_EMPTY_ENTRY;
    }
    write_blocks(&fat, cluster_to_lba(1), 4);
}

void initialize_filesystem_fat32(void)
{
    if (is_empty_storage())
    {
        create_fat32();
    }
    else
    {
        read_blocks(&fat32_driver_state, cluster_to_lba(1), 4);
    }
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    write_blocks(ptr, cluster_to_lba(cluster_number), cluster_to_lba(cluster_count));
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    read_blocks(ptr, cluster_to_lba(cluster_number), cluster_to_lba(cluster_count));
}

int8_t read_directory(struct FAT32DriverRequest request)
{
    if (fat32_driver_state.fat_table.cluster_map[request.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
        return -1;
    if (request.buffer_size != sizeof(struct FAT32DirectoryTable))
        return -1;
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    uint8_t code = 2;
    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0)
        {
            if (fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY)
            {
                uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
                read_clusters(request.buf, cluster_number, 1);
                return 0;
            }
            code = 1;
        }
    }
    return code;
}

int8_t read(struct FAT32DriverRequest request)
{
    if (fat32_driver_state.fat_table.cluster_map[request.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
        return -1;
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    uint8_t code = 2;

    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0)
        {
            if (!(fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY) && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
            {
                if (request.buffer_size < fat32_driver_state.dir_table_buf.table[i].filesize)
                    return -1;
                uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
                uint32_t idx = 0;
                while (cluster_number != FAT32_FAT_END_OF_FILE)
                {
                    read_clusters((uint8_t *)request.buf + CLUSTER_SIZE * idx, cluster_number, 1);
                    cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
                    idx++;
                }
                return 0;
            }
            code = 1;
        }
    }
    return code;
}

int8_t write(struct FAT32DriverRequest request)
{
    // validation
    if (fat32_driver_state.fat_table.cluster_map[request.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
        return 2;

    // read data from parent folder
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    uint32_t first = 0;

    // check if the request file name and extension already exist
    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
        {
            if (!first)
                first = i;
            continue;
        }
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
            return 1;
    }

    if (!first)
        return -1; // no available space in directory table

    fat32_driver_state.dir_table_buf.table[first].filesize = request.buffer_size;
    fat32_driver_state.dir_table_buf.table[first].user_attribute &= 0b01010101;
    memcpy(fat32_driver_state.dir_table_buf.table[first].name, request.name, sizeof(request.name));
    memcpy(fat32_driver_state.dir_table_buf.table[first].ext, request.ext, sizeof(request.ext));

    // if write folder
    if (request.buffer_size == 0)
    {
        for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
        {
            if (fat32_driver_state.fat_table.cluster_map[i] == FAT32_FAT_EMPTY_ENTRY)
            {
                fat32_driver_state.dir_table_buf.table[first].cluster_low = (uint16_t)i;
                fat32_driver_state.dir_table_buf.table[first].cluster_high = (uint16_t)(i >> 16);
                struct FAT32DirectoryTable dt;
                init_directory_table(&dt, request.name, request.parent_cluster_number);
                write_clusters(&dt, i, 1);
                break;
            }
        }
    }
    else
    {
        uint16_t num_of_cluster = (request.buffer_size / CLUSTER_SIZE) + (request.buffer_size % CLUSTER_SIZE != 0 ? 1 : 0);
        uint16_t idx = 0;  // idx of cluster
        uint32_t curr = 0; // current logical address
        for (unsigned i = 3; i < CLUSTER_MAP_SIZE && idx < num_of_cluster; i++)
        {
            if (fat32_driver_state.fat_table.cluster_map[i] == FAT32_FAT_EMPTY_ENTRY)
            {
                if (!curr)
                {
                    fat32_driver_state.dir_table_buf.table[first].cluster_low = (uint16_t)i;
                    fat32_driver_state.dir_table_buf.table[first].cluster_high = (uint16_t)(i >> 16);
                }
                else
                {
                    fat32_driver_state.fat_table.cluster_map[curr] = i;
                }
                curr = i;
                write_clusters((uint8_t *)request.buf + idx * CLUSTER_SIZE, i, 1);
                idx++;
            }
        }
        fat32_driver_state.fat_table.cluster_map[curr] = FAT32_FAT_END_OF_FILE;
    }
    write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    write_clusters(&fat32_driver_state, 1, 1);
    return 0;
}

int8_t delete(struct FAT32DriverRequest request)
{
    if (fat32_driver_state.fat_table.cluster_map[request.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
        return -1;

    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);

    struct ClusterBuffer b;
    for (int j = 0; j < 512 * 4; j++)
        b.buf[j] = 0;

    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
        {
            uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;

            // if delete folder
            if (fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY)
            {
                // check if folder is empty
                struct FAT32DirectoryTable folder_table;
                read_clusters(&folder_table, cluster_number, 1);

                for (unsigned j = 2; j < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); j++)
                {
                    if (folder_table.table[j].attribute & UATTR_NOT_EMPTY)
                        return 2;
                }
                write_clusters(&b, cluster_number, 1);
            }
            else
            {
                while (cluster_number != FAT32_FAT_END_OF_FILE)
                {
                    write_clusters(&b, cluster_number, 1);
                    cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
                }
            }
            memset(&fat32_driver_state.dir_table_buf.table[i], 0, sizeof(struct FAT32DirectoryEntry));
            write_clusters(&fat32_driver_state, 1, 1);
            return 0;
        }
    }
    return 1;
}