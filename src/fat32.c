#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"
#include "header/filesystem/fat32.h"

static struct FAT32DriverState fat32_driver_state;

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

uint32_t cluster_to_lba(uint32_t cluster) {
    return cluster * CLUSTER_BLOCK_COUNT;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster) {
    memcpy(dir_table->table[0].name, name, sizeof(name));
    dir_table->table[1].cluster_low = (uint16_t)parent_dir_cluster;
    dir_table->table[1].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
}

bool is_empty_storage(void) {
    struct BlockBuffer b;
    read_blocks(&b, BOOT_SECTOR, 1);
    return memcmp((const void *)&b, fs_signature, sizeof(fs_signature)) != 0;
}

void create_fat32(void) {
    write_blocks(fs_signature, BOOT_SECTOR, 1);
    struct FAT32FileAllocationTable fat;
    fat.cluster_map[0] = CLUSTER_0_VALUE;
    fat.cluster_map[1] = CLUSTER_1_VALUE;
    fat.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    for (int i = 3; i < CLUSTER_MAP_SIZE; i++) {
        fat.cluster_map[i] = FAT32_FAT_EMPTY_ENTRY;
    }
    write_blocks(&fat, cluster_to_lba(1), 4);
}

void initialize_filesystem_fat32(void) {
    if (is_empty_storage()) {
        create_fat32();
    }
    else {
        read_blocks(&fat32_driver_state, cluster_to_lba(1), 4);
    }
}