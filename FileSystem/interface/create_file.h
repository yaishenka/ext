/**
 * @file create_file.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains method to create file
 */
#ifndef EXT_FILESYSTEM_INTERFACE_CREATE_FILE_H_
#define EXT_FILESYSTEM_INTERFACE_CREATE_FILE_H_

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
#include "../../utils/utils.h"

/**
 * @brief Creates file
 * @param path_to_fs_file
 * @param path
 * @param output_fd
 */
void create_file(const char* path_to_fs_file, const char* path, int output_fd) {
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
  char dirname[buffer_length];

  if (!split_path(path, parent_path, dirname)) {
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
    return;
  }

  if (inode.inode_info->is_file) {
    buffered_write(&buffer, &buffer_size, "Trying to mkdir in file. Abort!\n", strlen("Trying to mkdir in file. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  if (is_dir_exist(fd, &inode, dirname, &superblock)) {
    buffered_write(&buffer, &buffer_size, "File already exist! Abort!\n", strlen("File already exist! Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  uint16_t new_inode_id = create_file_helper(fd, &superblock, inode_id);

  if (new_inode_id == superblock.fs_info->inodes_count) {
    buffered_write(&buffer, &buffer_size, "Can't create more inodes! Abort!\n", strlen("Can't create more inodes! Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  struct block block;
  if (read_block(fd, &block, inode.block_ids[0], &superblock) == -1) {
    buffered_write(&buffer, &buffer_size, "Can't read block. Abort!\n", strlen("Can't read block. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  if (block.block_info->records_count == get_max_records_count(&superblock)) {
    buffered_write(&buffer, &buffer_size, "Can't create more files in this dir. Abort!\n", strlen("Can't create more files in this dir. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  block.block_records = (struct block_record*) realloc(block.block_records,
                                                       (block.block_info->records_count
                                                           + 1)
                                                           * sizeof(struct block_record));
  struct block_record new_record;
  init_block_record(&new_record, &superblock, new_inode_id);
  strcpy(new_record.path, dirname);
  block.block_records[block.block_info->records_count] = new_record;
  block.block_info->records_count += 1;
  if (write_block(fd, &block, &superblock) == -1) {
    buffered_write(&buffer, &buffer_size, "Can't write block. Abort!\n", strlen("Can't write block. Abort!\n"));
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
    destroy_inode(&inode);
    destruct_block(&block);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  if (buffer != NULL) {
    write_while(STDERR_FILENO, buffer, buffer_size);
    send_data(output_fd, buffer, buffer_size);
    free(buffer);
  } else {
    send_data(output_fd, NULL, 0);
  }

  destroy_inode(&inode);
  destruct_block(&block);
  destroy_super_block(&superblock);
  close(fd);
}

#endif //EXT_FILESYSTEM_INTERFACE_CREATE_FILE_H_
