/** @author yaishenka
    @date 12.03.2021 */
#ifndef EXT_FILESYSTEM_INTERFACE_LS_H_
#define EXT_FILESYSTEM_INTERFACE_LS_H_
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "../core/defines.h"
#include "../core/superblock.h"
#include "../core/inode.h"
#include "../core/methods.h"

/**
 * @brief List directory
 * @param path_to_fs_file
 * @param path_to_dir
 * @warning Must be called only on initialized fs file
 */
void ls(const char* path_to_fs_file, const char* path_to_dir) {
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

  uint16_t inode_id = get_inode_id_of_dir(fd, path_to_dir, &superblock);

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
    fprintf(stderr, "Trying to list file. Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  struct block block;
  if (read_block(fd, &block, inode.block_ids[0], &superblock) == -1) {
    fprintf(stderr, "Can't read block. Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  destroy_inode(&inode);
  for (uint16_t record_id = 0; record_id < block.block_info->records_count;
       ++record_id) {
    printf("%s", block.block_records[record_id].path);

    if (read_inode(fd,
                   &inode,
                   block.block_records[record_id].inode_id,
                   &superblock) == -1) {
      fprintf(stderr, "Can't read inode. Abort!\n");
      destroy_super_block(&superblock);
      close(fd);
      return;
    }

    if (inode.inode_info->is_file) {
      printf(" -- file");
    }

    destroy_inode(&inode);

    printf("\n");
  }

  destruct_block(&block);
  destroy_super_block(&superblock);
  close(fd);
}

#endif //EXT_FILESYSTEM_INTERFACE_LS_H_
