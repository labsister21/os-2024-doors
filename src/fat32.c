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
    char *c = "..";
    memcpy(dir_table->table[0].name, name, 8);
    memcpy(dir_table->table[1].name, c, 2);

    dir_table->table[1].cluster_low = (uint16_t)parent_dir_cluster;
    dir_table->table[1].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[1].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[1].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[1].undelete = false;

    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[0].undelete = false;

    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        memset(&dir_table->table[i], 0, sizeof(struct FAT32DirectoryEntry));
    }
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

    fat32_driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat32_driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    fat32_driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    for (int i = 3; i < CLUSTER_MAP_SIZE; i++)
    {
        fat32_driver_state.fat_table.cluster_map[i] = FAT32_FAT_EMPTY_ENTRY;
    }
    init_directory_table(&fat32_driver_state.dir_table_buf, "root", 2);
    fat32_driver_state.dir_table_buf.table[0].cluster_high = 0;
    fat32_driver_state.dir_table_buf.table[0].cluster_low = 2;
    write_clusters(&fat32_driver_state.dir_table_buf, 2, 1);
    write_clusters(&fat32_driver_state, 1, 1);
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
    for (unsigned i = 1; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
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
            if ((fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY))
            {
                code = 1;
            }
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
    fat32_driver_state.dir_table_buf.table[first].user_attribute = 0b10101010;
    memcpy(fat32_driver_state.dir_table_buf.table[first].name, request.name, sizeof(request.name));
    memcpy(fat32_driver_state.dir_table_buf.table[first].ext, request.ext, sizeof(request.ext));

    // if write folder
    if (request.buffer_size == 0)
    {
        for (unsigned i = 3; i < CLUSTER_MAP_SIZE; i++)
        {
            if (fat32_driver_state.fat_table.cluster_map[i] == FAT32_FAT_EMPTY_ENTRY)
            {
                fat32_driver_state.dir_table_buf.table[first].attribute = ATTR_SUBDIRECTORY;
                fat32_driver_state.dir_table_buf.table[first].cluster_low = (uint16_t)i;
                fat32_driver_state.dir_table_buf.table[first].cluster_high = (uint16_t)(i >> 16);
                struct FAT32DirectoryTable dt;
                init_directory_table(&dt, request.name, request.parent_cluster_number);
                dt.table[0].cluster_low = (uint16_t)i;
                dt.table[0].cluster_high = (uint16_t)(i >> 16);
                write_clusters(&dt, i, 1);
                fat32_driver_state.fat_table.cluster_map[i] = FAT32_FAT_END_OF_FILE;
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
                    fat32_driver_state.dir_table_buf.table[first].attribute = 0;
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

void rec_delete(uint32_t dir_cluster)
{
    read_clusters(&fat32_driver_state.dir_table_buf, dir_cluster, 1);
    struct ClusterBuffer b;
    for (int j = 0; j < 512 * 4; j++)
        b.buf[j] = 0;
    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
        // if folder
        if (fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY)
        {
            rec_delete(cluster_number);
            read_clusters(&fat32_driver_state.dir_table_buf, dir_cluster, 1);
        }
        // if file
        else
        {
            while (cluster_number != FAT32_FAT_END_OF_FILE)
            {
                write_clusters(&b, cluster_number, 1);
                unsigned temp = fat32_driver_state.fat_table.cluster_map[cluster_number];
                fat32_driver_state.fat_table.cluster_map[cluster_number] = FAT32_FAT_EMPTY_ENTRY;
                cluster_number = temp;
            }
        }
    }
    write_clusters(&b, dir_cluster, 1);
    fat32_driver_state.fat_table.cluster_map[dir_cluster] = FAT32_FAT_EMPTY_ENTRY;
}

int8_t delete_folder_rec(struct FAT32DriverRequest request)
{
    if (fat32_driver_state.fat_table.cluster_map[request.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
        return -1;
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);

    uint32_t code = 2;

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
                // delete recursively
                rec_delete(cluster_number);

                read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
                memset(&fat32_driver_state.dir_table_buf.table[i], 0, sizeof(struct FAT32DirectoryEntry));
                write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
                write_clusters(&fat32_driver_state, 1, 1);
                return 0;
            }
            else
                code = 1;
        }
    }
    return code;
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
                    if (folder_table.table[j].user_attribute & UATTR_NOT_EMPTY)
                        return 2;
                }
                write_clusters(&b, cluster_number, 1);
                fat32_driver_state.fat_table.cluster_map[cluster_number] = FAT32_FAT_EMPTY_ENTRY;
            }
            else
            {
                while (cluster_number != FAT32_FAT_END_OF_FILE)
                {
                    write_clusters(&b, cluster_number, 1);
                    unsigned temp = fat32_driver_state.fat_table.cluster_map[cluster_number];
                    fat32_driver_state.fat_table.cluster_map[cluster_number] = FAT32_FAT_EMPTY_ENTRY;
                    cluster_number = temp;
                }
            }
            read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
            memset(&fat32_driver_state.dir_table_buf.table[i], 0, sizeof(struct FAT32DirectoryEntry));
            write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
            write_clusters(&fat32_driver_state, 1, 1);
            return 0;
        }
    }
    return 1;
}

int8_t search_file(struct FAT32DriverRequest request)
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
                return 0;
            }
            code = 1;
        }
    }
    return code;
}

uint32_t get_cluster_number(struct FAT32DriverRequest request)
{
    if (fat32_driver_state.fat_table.cluster_map[request.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
        return 0;
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0)
        {
            if (fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY)
            {
                uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
                return cluster_number;
            }
        }
    }
    return 0;
}

int8_t copy_folder(uint32_t src_cluster, uint32_t dest_cluster)
{
    if (fat32_driver_state.fat_table.cluster_map[src_cluster] != FAT32_FAT_END_OF_FILE)
        return -1;
    read_clusters(&fat32_driver_state.dir_table_buf, src_cluster, 1);
    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if (fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY)
        {
            // create folder in the dest cluster
            struct ClusterBuffer temp;
            struct FAT32DriverRequest req = {
                .buffer_size = 0,
                .buf = &temp,
                .ext = "\0\0\0",
                .parent_cluster_number = dest_cluster,
            };
            memcpy(req.name, fat32_driver_state.dir_table_buf.table[i].name, 8);
            uint32_t curr_src_cluster = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;

            write(req);
            uint32_t curr_dest_cluster = get_cluster_number(req);
            
            copy_folder(curr_src_cluster, curr_dest_cluster);
            read_clusters(&fat32_driver_state.dir_table_buf, src_cluster, 1);
        }
        else
        {
            // read src content
            int num_of_cluster = (fat32_driver_state.dir_table_buf.table[i].filesize / CLUSTER_SIZE) + (fat32_driver_state.dir_table_buf.table[i].filesize % CLUSTER_SIZE != 0 ? 1 : 0);
            struct ClusterBuffer buf[num_of_cluster];
            uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
            uint32_t idx = 0;
            while (cluster_number != FAT32_FAT_END_OF_FILE)
            {
                read_clusters((uint8_t *)buf + CLUSTER_SIZE * idx, cluster_number, 1);
                cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
                idx++;
            }

            // copy to target
            struct FAT32DriverRequest req = {
                .buf = buf,
                .parent_cluster_number = dest_cluster,
                .buffer_size = fat32_driver_state.dir_table_buf.table[i].filesize,
            };
            memcpy(req.ext, fat32_driver_state.dir_table_buf.table[i].ext, 3);
            memcpy(req.name, fat32_driver_state.dir_table_buf.table[i].name, 8);
            write(req);
            read_clusters(&fat32_driver_state.dir_table_buf, src_cluster, 1);
        }
    }
    return 0;
}

int8_t copy_file(struct FAT32DriverRequest src, struct FAT32DriverRequest dest)
{
    if (fat32_driver_state.fat_table.cluster_map[src.parent_cluster_number] != FAT32_FAT_END_OF_FILE)
        return -1;

    read_clusters(&fat32_driver_state.dir_table_buf, src.parent_cluster_number, 1);

    uint32_t filesize;
    uint32_t num_of_cluster;
    uint32_t cluster_number = 0;

    // get src file size
    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, src.name, 8) == 0)
        {
            if (!(fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY) && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, src.ext, 3) == 0)
            {
                filesize = fat32_driver_state.dir_table_buf.table[i].filesize;
                num_of_cluster = (fat32_driver_state.dir_table_buf.table[i].filesize / CLUSTER_SIZE) + (fat32_driver_state.dir_table_buf.table[i].filesize % CLUSTER_SIZE != 0 ? 1 : 0);
                cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
                break;
            }
        }
    }
    if (!cluster_number)
        return 1;

    // read data
    struct ClusterBuffer buf[num_of_cluster];
    uint32_t idx = 0;
    while (cluster_number != FAT32_FAT_END_OF_FILE)
    {
        read_clusters((uint8_t *)buf + CLUSTER_SIZE * idx, cluster_number, 1);
        cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
        idx++;
    }

    // check if destination file exist
    read_clusters(&fat32_driver_state.dir_table_buf, dest.parent_cluster_number, 1);
    idx = 0;
    for (unsigned i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (!(fat32_driver_state.dir_table_buf.table[i].user_attribute & UATTR_NOT_EMPTY))
            continue;
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, dest.name, 8) == 0)
        {
            if (!(fat32_driver_state.dir_table_buf.table[i].attribute & ATTR_SUBDIRECTORY) && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, dest.ext, 3) == 0)
            {
                idx = i;
                break;
            }
        }
    }

    struct FAT32DriverRequest req = {
        .buf = buf,
        .parent_cluster_number = dest.parent_cluster_number,
        .buffer_size = filesize,
    };
    memcpy(req.name, dest.name, 8);
    memcpy(req.ext, dest.ext, 3);

    if (idx)
        delete (req);
    return write(req);
    ;
}