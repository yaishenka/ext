/**
 * @file open_file.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains method to open file
 */
#ifndef EXT_FILESYSTEM_INTERFACE_OPEN_FILE_H_
#define EXT_FILESYSTEM_INTERFACE_OPEN_FILE_H_
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
 * @brief Open file and printf fd
 * @param path_to_fs_file
 * @param path
 */
void open_file(const char* path_to_fs_file, const char* path, int output_fd) {
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

  char parent_path[buffer_length];
  char filename[buffer_length];

  if (!split_path(path, parent_path, filename)) {
    buffered_write(&buffer, &buffer_size, "Incorrect path. Abort!\n", strlen("Incorrect path. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  uint16_t inode_id = get_inode_id_of_dir(fd, parent_path, &superblock);
  if (inode_id == superblock.fs_info->inodes_count) {
    buffered_write(&buffer, &buffer_size, "Can't find directory. Abort!\n", strlen("Can't find directory. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  struct inode inode;
  if (read_inode(fd, &inode, inode_id, &superblock) == -1) {
    buffered_write(&buffer, &buffer_size, "Can't read inode. Abort!\n", strlen("Can't read inode. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_super_block(&superblock);
    close(fd);
  }

  if (inode.inode_info->is_file) {
    buffered_write(&buffer, &buffer_size, "File doesn't exist. Abort!\n", strlen("File doesn't exist. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  struct descriptors_table descriptors_table;
  if (read_descriptors_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't read descriptors_table. Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  uint16_t file_inode_id = get_file_inode_id(fd, &inode, filename, &superblock);

  if (file_inode_id == superblock.fs_info->inodes_count) {
    buffered_write(&buffer, &buffer_size, "File doesn't exist. Abort!\n", strlen("File doesn't exist. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  int new_fd =
      reserve_descriptor(&descriptors_table, file_inode_id, &superblock);

  if (new_fd == -1) {
    buffered_write(&buffer, &buffer_size, "Can't open file. Abort!\n", strlen("Can't open file. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't write descriptors_table. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  char string_buffer[1024];
  size_t string_size = sprintf(string_buffer, "opened fd: %d\n", new_fd);
  buffered_write(&buffer, &buffer_size, string_buffer, strlen(string_buffer));
  write_while(STDERR_FILENO, buffer, buffer_size);
  send_data(output_fd, buffer, buffer_size);
  free(buffer);


  destruct_descriptors_table(&descriptors_table, &superblock);
  destroy_inode(&inode);
  destroy_super_block(&superblock);
  close(fd);
}

#endif //EXT_FILESYSTEM_INTERFACE_OPEN_FILE_H_
