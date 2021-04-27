/**
 * @file lseek_pos.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains method to lseek
 */
#ifndef EXT_FILESYSTEM_INTERFACE_LSEEK_POS_H_
#define EXT_FILESYSTEM_INTERFACE_LSEEK_POS_H_
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

void lseek_pos(const char* path_to_fs_file,
               uint16_t file_descriptor,
               uint32_t pos, int output_fd) {
  char* buffer = NULL;
  size_t buffer_size = 0;
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

  if (!descriptors_table.reserved_fd[file_descriptor]) {
    buffered_write(&buffer, &buffer_size, "Descriptor is closed. Abort!\n", strlen("Descriptor is closed. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    return;
  }

  if (pos >= get_max_data_size_of_all_blocks(&superblock)) {
    buffered_write(&buffer, &buffer_size, "Position >= max_data_in_file. Abort!\n", strlen("Position >= max_data_in_file. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    return;
  }

  descriptors_table.fd_to_position[file_descriptor] = pos;

  if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't write descriptors_table. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (buffer != NULL) {
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
  } else {
    send_data(output_fd, NULL, 0);
  }

  destruct_descriptors_table(&descriptors_table, &superblock);
  destroy_super_block(&superblock);
  close(fd);
}

#endif //EXT_FILESYSTEM_INTERFACE_LSEEK_POS_H_
