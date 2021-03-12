/** @author yaishenka
    @date 12.03.2021 */
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
#include "../utils.h"

/**
 * @brief Open file and printf fd
 * @param path_to_fs_file
 * @param path
 */
void open_file(const char* path_to_fs_file, const char* path) {
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
    fprintf(stderr, "Incorrect path. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  uint16_t inode_id = get_inode_id_of_dir(fd, parent_path, &superblock);
  if (inode_id == superblock.fs_info->inodes_count) {
    fprintf(stderr, "Can't find directory. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  struct inode inode;
  if (read_inode(fd, &inode, inode_id, &superblock) == -1) {
    fprintf(stderr, "Can't read inode. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
  }

  if (inode.inode_info->is_file) {
    fprintf(stderr, "File doesn't exist. Abort!\n");
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

  uint16_t file_inode_id = get_file_inode_id(fd, &inode, filename, &superblock) ;

  if (file_inode_id == superblock.fs_info->inodes_count) {
    fprintf(stderr, "File doesn't exist. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  int new_fd = reserve_descriptor(&descriptors_table, file_inode_id, &superblock);

  if (new_fd == -1) {
    fprintf(stderr, "Can't open file. Abort!\n");
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

  printf("opened fd: %d\n", new_fd);
  destruct_descriptors_table(&descriptors_table, &superblock);
  destroy_inode(&inode);
  destroy_super_block(&superblock);
  close(fd);
}

#endif //EXT_FILESYSTEM_INTERFACE_OPEN_FILE_H_
