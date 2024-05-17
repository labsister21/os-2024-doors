#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include <stddef.h>
#include "../header/filesystem/fat32.h"
#include "syscall.h"

struct ShellState
{
  uint32_t work_dir;
  char curr_command_buffer[256];
  uint32_t curr_command_size;
  char work_dir_name[256];
};

void print_working_dir();

void type_command();

void run_command();

void ls();

void cd(char *name);

void mkdir(char *name);

void move_back(char *c);

void cp(char *src, char *dest);

void cp_rec(char *src, char *dest);

void cat(char *filename);

void rm(char *filename);

void rm_rec(char *foldername);

void mv(char *src, char *dest);

void help();

void print_int(uint32_t num);

void find(uint32_t cluster_number, char name[8], char ext[3], bool *isFound, char curr_path[256]);

void exec(char *filename);

void kill(char *pid);

void ps();

/**
 * @return int8_t
 * 0: target is file and exist
 * 1: target is folder and exist
 * 2: target does not exist
 * 3: invalid path
 * 4: name length > 8
 * 5: ext length > 3
*/

int8_t get_curr_and_parent_cluster(char *path, uint32_t *parent_cluster, uint32_t *current_cluster, char* filename, char* ext);

#endif