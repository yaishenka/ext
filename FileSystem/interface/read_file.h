/** @author yaishenka
    @date 13.03.2021 */
#ifndef EXT_FILESYSTEM_INTERFACE_READ_FILE_H_
#define EXT_FILESYSTEM_INTERFACE_READ_FILE_H_

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
 * @brief Read data from file
 * @param path_to_fs_file
 * @param file_descriptor opened file descriptor from our FS
 * @param place_to_read
 * @param size
 * @return count of readed_bytes
 */
ssize_t read_file(const char* path_to_fs_file,
                  uint16_t file_descriptor,
                  char* dest,
                  uint32_t size) {
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

  if (size > get_max_data_size_of_all_blocks(&superblock)) {
    size = get_max_data_size_of_all_blocks(&superblock);
  }

  struct descriptors_table descriptors_table;
  if (read_descriptors_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't read descriptors_table. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (!descriptors_table.reserved_fd[file_descriptor]) {
    fprintf(stderr, "Trying to read from closed fd. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    close(fd);
    return -1;
  }

  uint32_t fd_position = descriptors_table.fd_to_position[file_descriptor];
  uint16_t inode_id = descriptors_table.fd_to_inode[file_descriptor];

  struct inode inode;
  if (read_inode(fd, &inode, inode_id, &superblock) == -1) {
    fprintf(stderr, "Can't read inode. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  uint32_t total_read = 0;
  uint32_t need_to_read = size;

  while (total_read != size) {
    uint16_t block_to_read_pos =
        (uint16_t) fd_position / get_max_data_in_block(&superblock);
    if (block_to_read_pos > inode.inode_info->blocks_count) {
      break;
    }

    struct block block;
    if (read_block(fd,
                   &block,
                   inode.block_ids[block_to_read_pos],
                   &superblock) == -1) {
      fprintf(stderr, "Can't read block. Abort!\n");
      destruct_descriptors_table(&descriptors_table, &superblock);
      destroy_inode(&inode);
      destroy_super_block(&superblock);
      close(fd);
      exit(EXIT_FAILURE);
    }

    uint32_t position_in_block_data =
        (uint32_t) fd_position % get_max_data_in_block(&superblock);

    char* position_to_read = block.data + position_in_block_data;
    uint32_t remain_read = block.block_info->data_size - position_in_block_data;
    if (remain_read == 0) {
      break;
    }
    uint32_t
        size_to_read = need_to_read < remain_read ? need_to_read : remain_read;
    memcpy(dest, position_to_read, size_to_read);
    fd_position += size_to_read;
    dest += size_to_read;
    total_read += size_to_read;
    need_to_read -= size_to_read;
  }

  descriptors_table.fd_to_position[file_descriptor] = fd_position;
  if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't write descriptor table. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }
  destruct_descriptors_table(&descriptors_table, &superblock);
  destroy_inode(&inode);
  destroy_super_block(&superblock);
  close(fd);

  printf("Total readed: %d\n", total_read);

  return total_read;
}

/**
 * @brief Read data from file
 * Read data from file and put it to path
 * @param path_to_fs_file
 * @param file_descriptor
 * @param path
 * @param size if size == -1 file will be readed till end
 */
void read_file_to_file(const char* path_to_fs_file,
                       uint16_t file_descriptor,
                       const char* path, ssize_t size) {
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

  ssize_t max_size = get_max_data_size_of_all_blocks(&superblock);
  max_size = size == -1 ? max_size : (size > max_size ? max_size : size);
  destroy_super_block(&superblock);
  close(fd);

  char* buffer = calloc(max_size, sizeof(char));
  ssize_t total_read =
      read_file(path_to_fs_file, file_descriptor, buffer, max_size);

  if (total_read == -1) {
    free(buffer);
    return;
  }

  fd = open(path, O_RDWR | O_CREAT);

  if (fd == -1) {
    fprintf(stderr, "Can't open file to write. Abort!\n");
    free(buffer);
    return;
  }

  ssize_t written = write_while(fd, buffer, total_read);
  free(buffer);
  close(fd);

  printf("Written %zd to %s\n", written, path);
}

#endif //EXT_FILESYSTEM_INTERFACE_READ_FILE_H_
