/** @author yaishenka
    @date 12.03.2021 */
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
#include "../utils.h"

/**
 * @brief Creates file
 * @param path_to_fs_file
 * @param path
 */
void create_file(const char* path_to_fs_file, const char* path) {
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
    fprintf(stderr, "Trying to mkdir in file. Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  if (is_dir_exist(fd, &inode, dirname, &superblock)) {
    fprintf(stderr, "Dir already exist! Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  uint16_t new_inode_id = create_file_helper(fd, &superblock, inode_id);

  if (new_inode_id == superblock.fs_info->inodes_count) {
    fprintf(stderr, "Can't create more inodes! Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
  }

  struct block block;
  if (read_block(fd, &block, inode.block_ids[0], &superblock) == -1) {
    fprintf(stderr, "Can't read block. Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
  }

  if (block.block_info->records_count == get_max_records_count(&superblock)) {
    fprintf(stderr, "Can't create more files in this dir. Abort!\n");
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
    fprintf(stderr, "Can't write block. Abort!\n");
    destroy_inode(&inode);
    destruct_block(&block);
    destroy_super_block(&superblock);
    close(fd);
  }

  destroy_inode(&inode);
  destruct_block(&block);
  destroy_super_block(&superblock);
  close(fd);
}

#endif //EXT_FILESYSTEM_INTERFACE_CREATE_FILE_H_
