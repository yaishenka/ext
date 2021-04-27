/**
 * @file init.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains methods to init FS
 */
#ifndef EXT_FILESYSTEM_INTERFACE_INIT_H_
#define EXT_FILESYSTEM_INTERFACE_INIT_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "../core/superblock.h"
#include "../core/descriptors_table.h"
#include "../core/defines.h"
#include "../core/methods.h"
#include "utils.h"
#include "net_utils.h"

/**
 * @brief Init filesystem
 * Trunc file and init our FS in it. Creates superblock, descriptor_table and root_dir
 * @param path_to_fs_file
 */
void init_fs(const char* path_to_fs_file) {
  int fd = open(path_to_fs_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    fprintf(stderr, "Can't open file. Abort!");
    exit(EXIT_FAILURE);
  }

  struct superblock superblock;
  init_super_block(&superblock);
  if (write_super_block(fd, &superblock) == -1) {
    destroy_super_block(&superblock);
    fprintf(stderr, "Can't write superblock. Abort!");
    exit(EXIT_FAILURE);
  }

  struct descriptors_table descriptors_table;
  init_descriptors_table(&descriptors_table, &superblock);
  if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't write descriptors_table. Abort!");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    exit(EXIT_FAILURE);
  }

  create_dir_helper(fd, &superblock, 0, true);

  destruct_descriptors_table(&descriptors_table, &superblock);
  destroy_super_block(&superblock);
  close(fd);
}

void read_fs(const char* path_to_fs_file) {
  int fd = open(path_to_fs_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    fprintf(stderr, "Can't open file. Abort!");
    exit(EXIT_FAILURE);
  }
  struct superblock superblock;
  read_super_block(fd, &superblock);
  if (superblock.fs_info->magic != MAGIC) {
    fprintf(stderr, "Magic does not match. Abort!");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }
  destroy_super_block(&superblock);
  close(fd);
}

#endif //EXT_FILESYSTEM_INTERFACE_INIT_H_
