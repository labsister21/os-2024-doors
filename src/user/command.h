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

void cp(char * src, char * dest);

void cp_rec(char * src, char * dest);

void cat(char *filename);

void rm(char *filename);

void rm_rec(char * foldername);

void help();

void print_int(uint32_t num);

#endif