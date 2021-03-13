/**
 * @file close_file.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains method to close FD
 */
#ifndef EXT_FILESYSTEM_INTERFACE_CLOSE_FILE_H_
#define EXT_FILESYSTEM_INTERFACE_CLOSE_FILE_H_
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
#include "../utils.h"

/**
 * @brief Close file
 * @param path_to_fs_file
 * @param path
 */
void close_file(const char* path_to_fs_file, const int fd_to_close) {
  int fd = open(path_to_fs_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    fprintf(stderr, "Can't open file. Abort!\n");
    exit(EXIT_FAILURE);
  }

  struct superblock superblock;
  read_super_block(fd, &superblock);
  if (superblock.fs_info->magic != MAGIC) {
    fprintf(stderr, "Magic does not match. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (!superblock.reserved_inodes_mask[ROOT_INODE_ID]) {
    fprintf(stderr, "Root directory doesn't exist. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  struct descriptors_table descriptors_table;
  if (read_descriptors_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't read descriptors_table. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  free_descriptor(&descriptors_table, fd_to_close, &superblock);
  if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't write descriptors_table. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  destruct_descriptors_table(&descriptors_table, &superblock);
  destroy_super_block(&superblock);
  close(fd);
}
#endif //EXT_FILESYSTEM_INTERFACE_CLOSE_FILE_H_
